"""Nautilai Local Analysis."""

import argparse
import dataclasses
from dataclasses import dataclass
import json
import logging
import os
import sys
from typing import Any
import zipfile
from xlsxwriter import Workbook

import cv2 as cv
from matplotlib.backends.backend_pdf import PdfPages
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image, ImageDraw, ImageFont
import polars as pl
import pyarrow.parquet as pq
from scipy.stats import linregress
import toml

logger = logging.getLogger(__name__)

def _get_well_row_name(row: int):
    well_name = chr(ord("A") + row % 26)
    if row >= 26:
        well_name = "A" + well_name
    return well_name


MAX_ROWS = 32
ALL_WELL_ROWS = tuple([_get_well_row_name(row) for row in range(MAX_ROWS)])


LED_INTENSITY_COL = "Background Fluorescence, {}% LED Intensity (AU)"
LED_INTENSITIES = (0.25, 0.5, 1)


@dataclass
class Point:
    x: int
    y: int


@dataclass
class RoiCoords:
    p_ul: Point  # upper left
    p_br: Point  # bottom right


@dataclass
class BackgroundRecordingInfo:
    data: pl.DataFrame
    metadata: dict[str, Any]


class RawDataReader:
    def __init__(
        self, file_path: str, num_frames: int, frame_shape: tuple[int, int], dtype: np.dtype
    ) -> None:
        self._file_path: str = file_path
        self._num_frames: int = num_frames
        self._frame_shape: tuple[int, int] = frame_shape
        self._dtype: np.dtype = dtype

        self._frame_size_px = self._frame_shape[0] * self._frame_shape[1]
        self._frame_size_bytes = self._frame_size_px * self._dtype.itemsize

    def frame(self, idx: int) -> np.ndarray:
        if idx >= self._num_frames:
            raise IndexError(f"{idx} exceeds number of frames ({self._num_frames})")

        # TODO look into a way to keep the file open?
        return np.fromfile(
            file=self._file_path,
            dtype=self._dtype,
            count=self._frame_size_px,
            offset=self._frame_size_bytes * idx,
        ).reshape(self._frame_shape)

    def __iter__(self) -> "RawDataReader":
        self._iter = 0
        return self

    def __next__(self) -> np.ndarray:
        if self._iter >= self._num_frames:
            raise StopIteration

        frame = self.frame(self._iter)
        self._iter += 1
        return frame


# have to subclass and override this method to gain control over what happens when there is an issue 
# with the provided arguments
class ArgParse(argparse.ArgumentParser):
    def error(self, message):
        logger.error(message)
        sys.exit(2)


def main():
    logging.basicConfig(
        format="[%(asctime)s.%(msecs)03d] [local_analysis] [%(levelname)s] %(message)s",
        level=logging.INFO,
        datefmt="%Y-%m-%d %H:%M:%S",
        stream=sys.stdout
    )

    logger.info("Nautilai Local Analysis Starting")

    parser = ArgParse(description="Extracts signals from a multi-well microscope experiment")
    parser.add_argument(
        "toml_config_path", type=str, default=None, help="Path to a toml file with run config parameters"
    )

    cmd_line_args = parser.parse_args()

    with open(cmd_line_args.toml_config_path) as toml_file:
        setup_config = toml.load(toml_file)

    logger.info(f"Metadata: {setup_config}")

    setup_config["recording_name"] = os.path.splitext(os.path.basename(setup_config["input_path"]))[0]

    _scale_inputs(setup_config)

    raw_data_reader = _create_raw_data_reader(setup_config)

    rois = _create_rois(setup_config)
    _create_roi_annotated_image(raw_data_reader, setup_config, rois)

    time_series_df = _calculate_fluorescence_time_series(raw_data_reader, rois, setup_config)

    if plate_id := setup_config["plate_id"]:
        background_recording_info = _load_background(plate_id)
        time_series_df = _subtract_background(time_series_df, setup_config, background_recording_info)

    _write_time_series_parquet(time_series_df, setup_config)
    _write_time_series_csv(time_series_df, setup_config)
    _create_time_series_plot_image(time_series_df, setup_config)
    _write_time_series_legacy_xlsx_zip(time_series_df, setup_config)


def _scale_inputs(setup_config: dict[str, Any]) -> None:
    if "additional_bin_factor" not in setup_config:
        setup_config["additional_bin_factor"] = 1

    setup_config["scaled"] = {
        "xy_pixel_size": (
            setup_config["xy_pixel_size"]
            * setup_config["additional_bin_factor"]
            * setup_config["scale_factor"]
        ),
        "height": setup_config["height"] // setup_config["additional_bin_factor"],
        "width": setup_config["width"] // setup_config["additional_bin_factor"],
        "num_horizontal_pixels": (
            setup_config["num_horizontal_pixels"] // setup_config["additional_bin_factor"]
        ),
        "num_vertical_pixels": setup_config["num_vertical_pixels"] // setup_config["additional_bin_factor"],
        "roi_size_x": (
            setup_config["stage"]["roi_size_x"]
            // setup_config["additional_bin_factor"]
            // setup_config["scale_factor"]
        ),
        "roi_size_y": (
            setup_config["stage"]["roi_size_y"]
            // setup_config["additional_bin_factor"]
            // setup_config["scale_factor"]
        ),
        "h_offset": setup_config["stage"]["h_offset"] // setup_config["scale_factor"],
        "v_offset": setup_config["stage"]["v_offset"] // setup_config["scale_factor"],
    }


def _create_raw_data_reader(setup_config: dict[str, Any]) -> np.ndarray:
    match setup_config["bit_depth"]:
        case 8:
            dtype = np.dtype(np.uint8)
        case 12 | 16:
            dtype = np.dtype("<u2")
        case invalid_bit_depth:
            raise ValueError(f"Invalid bit depth: {invalid_bit_depth}")

    raw_data_reader = RawDataReader(
        setup_config["input_path"],
        setup_config["num_frames"],
        (setup_config["scaled"]["num_vertical_pixels"], setup_config["scaled"]["num_horizontal_pixels"]),
        dtype,
    )

    return raw_data_reader


def _create_rois(setup_config: dict[str, Any]) -> dict[str, RoiCoords]:
    logger.info("Creating ROIs")

    well_spacing = setup_config["stage"]["well_spacing"]
    num_wells_h = setup_config["stage"]["num_wells_h"]
    num_wells_v = setup_config["stage"]["num_wells_v"]

    scaled_px_size = setup_config["scaled"]["xy_pixel_size"]
    scaled_roi_size_x = setup_config["scaled"]["roi_size_x"]
    scaled_roi_size_y = setup_config["scaled"]["roi_size_y"]
    scaled_height = setup_config["scaled"]["height"]
    scaled_width = setup_config["scaled"]["width"]

    rois = {}

    # TODO clean this up
    for tile_v_idx in range(setup_config["rows"]):
        plate_well_row_offset = tile_v_idx * num_wells_v
        for tile_h_idx in range(setup_config["cols"]):
            plate_well_col_offset = tile_h_idx * num_wells_h
            for well_v_idx in range(num_wells_v):
                for well_h_idx in range(num_wells_h):
                    well_x_center = (
                        scaled_width / 2
                        - ((num_wells_h - 1) / 2 - well_h_idx) * (well_spacing / scaled_px_size)
                        + tile_h_idx * (scaled_width)
                        + setup_config["scaled"]["h_offset"]
                    )
                    well_y_center = (
                        scaled_height / 2
                        - ((num_wells_v - 1) / 2 - well_v_idx) * (well_spacing / scaled_px_size)
                        + tile_v_idx * (scaled_height)
                        + setup_config["scaled"]["v_offset"]
                    )

                    well_name = ALL_WELL_ROWS[well_v_idx + plate_well_row_offset] + str(
                        well_h_idx + plate_well_col_offset + 1
                    )

                    rois[well_name] = RoiCoords(
                        Point(
                            int(well_x_center - scaled_roi_size_x / 2),
                            int(well_y_center - scaled_roi_size_y / 2),
                        ),
                        Point(
                            int(well_x_center + scaled_roi_size_x / 2),
                            int(well_y_center + scaled_roi_size_y / 2),
                        ),
                    )

    return rois


def _create_roi_annotated_image(
    raw_data_reader: RawDataReader, setup_config: dict[str, Any], rois: dict[str, RoiCoords]
) -> None:
    logger.info("Creating ROI sanity check image")

    # second frame is used for the roi annotation
    first_frame = raw_data_reader.frame(1)

    # scale values down to uint8 so PIL can process them
    if setup_config["bit_depth"] in (12, 16):
        first_frame = (first_frame / first_frame.max() * 255).astype("uint8")

    # TODO why is this necessary? Assuming it's for the colors
    first_frame = np.stack((first_frame, first_frame, first_frame), axis=-1)

    for well_name, roi in rois.items():
        cv.rectangle(
            img=first_frame,
            pt1=dataclasses.astuple(roi.p_ul),
            pt2=dataclasses.astuple(roi.p_br),
            color=(0, 255, 0),  # gbr
            thickness=1,
            lineType=cv.LINE_AA,
        )

    first_frame_image = Image.fromarray(first_frame)
    draw = ImageDraw.Draw(first_frame_image)

    font_size_px = 15
    try:
        font = ImageFont.truetype("arial.ttf", font_size_px)
    except OSError:
        font = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", font_size_px)

    for well_name, roi in rois.items():
        draw.text((roi.p_ul.x, roi.p_br.y), well_name, fill="rgb(0, 255, 0)", font=font)

    roi_output_path = os.path.join(
        setup_config["output_dir_path"], f"{setup_config['recording_name']}_roi_locations.png"
    )
    first_frame_image.save(roi_output_path)


def _calculate_fluorescence_time_series(
    raw_data_reader: RawDataReader, rois: dict[str, RoiCoords], setup_config: dict[str, Any]
) -> pl.DataFrame:
    logger.info("Creating fluorescence time series")

    well_arrs = {well_name: np.empty(setup_config["num_frames"]) for well_name in rois}

    for frame_idx, frame in enumerate(raw_data_reader):
        for well_name, well_arr in well_arrs.items():
            well_roi = rois[well_name]
            roi_arr = frame[well_roi.p_ul.y : well_roi.p_br.y, well_roi.p_ul.x : well_roi.p_br.x]
            well_arr[frame_idx] = np.mean(roi_arr)

    timepoints = np.arange(setup_config["num_frames"]) / setup_config["fps"]

    return pl.DataFrame({"time": timepoints} | well_arrs)


def _load_background(plate_id: str) -> BackgroundRecordingInfo:
    user_profile = os.getenv("USERPROFILE", r"C:\Users")
    bg_recording_dir = os.path.join(user_profile, "AppData", "Local", "Nautilai", "BackgroundRecordings")
    if not os.path.exists(bg_recording_dir):
        raise Exception("Background recording dir does not exist")

    plate_id_dir_path = None
    for dir_name in os.listdir(bg_recording_dir):
        if "_".join(dir_name.split("_")[:-1]) == plate_id:
            plate_id_dir_path = os.path.join(bg_recording_dir, dir_name)
    if plate_id_dir_path is None:
        raise Exception("Background recording dir for plate ID not found")

    bg_rec_data_path = os.path.join(plate_id_dir_path, f"{plate_id}.tsv")
    bg_rec_data = pl.read_csv(bg_rec_data_path, has_header=True, separator="\t")

    bg_rec_metadata_path = os.path.join(plate_id_dir_path, "settings.toml")
    with open(bg_rec_metadata_path) as toml_file:
        bg_rec_metadata = toml.load(toml_file)

    return BackgroundRecordingInfo(data=bg_rec_data, metadata=bg_rec_metadata)


def _subtract_background(
    time_series_df: pl.DataFrame, setup_config: dict[str, Any], bg_recording_info: BackgroundRecordingInfo
):
    bg_recording = bg_recording_info.data

    if set(time_series_df.drop("time").columns) != set(bg_recording["Well"]):
        raise Exception("Plate format of bg recording does not match")

    recording_led_intensity = setup_config["led_intensity"]
    bg_recording_led_intensity = bg_recording_info.metadata["led_intensity"]

    recording_exposure_dur = 1 / setup_config["fps"]
    bg_recording_exposure_dur = 1 / bg_recording_info.metadata["fps"]

    # scale to LED intensity
    bg_fluorescence = None
    for intensity_ratio in LED_INTENSITIES:
        if recording_led_intensity == bg_recording_led_intensity * intensity_ratio:
            intensity_col_name = LED_INTENSITY_COL.format(int(intensity_ratio * 100))
            bg_fluorescence = bg_recording.select("Well", intensity_col_name).transpose(column_names="Well")
    if bg_fluorescence is None:
        bg_data = (
            bg_recording.rename({LED_INTENSITY_COL.format(int(i * 100)): str(i) for i in LED_INTENSITIES})
            .transpose(include_header=True, header_name="intensity", column_names="Well")
            .cast({"intensity": float})
            .sort("intensity")
        )

        bg_fluorescence = pl.DataFrame()
        intensities = bg_data["intensity"]
        intensity_ratio = recording_led_intensity / bg_recording_led_intensity
        for well in bg_data.drop("intensity").columns:
            linregress_info = linregress(intensities, bg_data[well])
            bg_f_well = intensity_ratio * linregress_info.slope + linregress_info.intercept
            bg_fluorescence = bg_fluorescence.with_columns(**{well: pl.Series([bg_f_well])})

    # scale to frame rate
    bg_fluorescence *= recording_exposure_dur / bg_recording_exposure_dur

    # subtract background fluorescence from each well
    time_series_df = time_series_df.with_columns(
        [pl.col(c) - bg_fluorescence[c] for c in time_series_df.columns if c != "time"]
    )

    return time_series_df


def _write_time_series_parquet(time_series_df: pl.DataFrame, setup_config: dict[str, Any]) -> None:
    logger.info("Writing parquet output file")

    barcode_field = setup_config.get("plate_id")
    if not barcode_field:
        barcode_field = "N/A"

    metadata = {
        "utc_beginning_recording": setup_config["recording_date"],
        "file_format_version": "0.1.0",
        "instrument_type": "nautilai",
        "instrument_serial_number": "N/A",
        "software_release_version": setup_config["software_version"],
        "plate_barcode": barcode_field,
        "total_well_count": setup_config["stage"]["num_wells"],
        "stage_config": setup_config["stage"],
        "tissue_sampling_period": 1 / setup_config["fps"],
        "image_dimensions": (setup_config["num_horizontal_pixels"], setup_config["num_vertical_pixels"]),
    } | {
        key: setup_config[key]
        for key in (
            "data_type",
            "auto_contrast_brightness",
            "led_intensity",
            "bit_depth",
            "xy_pixel_size",
            "scale_factor",
            "additional_bin_factor",
            "vflip",
            "hflip",
            "auto_tile",
        )
    }

    # have to convert to pyarrow table to update metadata before writing parquet file
    time_series_table = time_series_df.to_arrow()
    time_series_table = time_series_table.replace_schema_metadata({"metadata": json.dumps(metadata)})

    time_series_pq_output_path = os.path.join(
        setup_config["output_dir_path"], f"{setup_config['recording_name']}.parquet"
    )
    pq.write_table(time_series_table, time_series_pq_output_path)


def _write_time_series_csv(time_series_df: pl.DataFrame, setup_config: dict[str, Any]) -> None:
    logger.info("Writing csv output file")

    time_series_csv_output_path = os.path.join(
        setup_config["output_dir_path"], f"{setup_config['recording_name']}.csv"
    )

    time_series_df.write_csv(time_series_csv_output_path)


def _create_time_series_plot_image(time_series_df: pl.DataFrame, setup_config: dict[str, Any]):
    logger.info("Writing plot pdf")

    time_series_plot_image_output_path = os.path.join(
        setup_config["output_dir_path"], f"{setup_config['recording_name']}_roi_signals_plots.pdf"
    )

    plate_rows = ALL_WELL_ROWS[: setup_config["rows"] * setup_config["stage"]["num_wells_v"]]
    plate_cols = list(range(1, setup_config["cols"] * setup_config["stage"]["num_wells_h"] + 1))

    with PdfPages(time_series_plot_image_output_path) as pdf_file:
        fig, axs = plt.subplots(len(plate_rows), len(plate_cols))

        fig.suptitle(
            f"Nautilai Experiment data - {setup_config['recording_date']} - {setup_config['recording_name']}",
            fontsize=50,
        )
        fig.set_size_inches(len(plate_cols) * 8, len(plate_rows) * 5)
        fig.tight_layout(pad=5)

        for row_idx, row in enumerate(plate_rows):
            for col_idx, col in enumerate(plate_cols):
                well_name = f"{row}{col}"
                ax = axs[row_idx, col_idx]
                ax.plot(time_series_df["time"], time_series_df[well_name])
                ax.set_title(well_name, fontsize=30)
                ax.tick_params(labelsize=20)

        pdf_file.savefig()


def _write_time_series_legacy_xlsx_zip(time_series_df: pl.DataFrame, setup_config: dict):
    wells = [c for c in time_series_df.columns if c != "time"]

    output_dir = os.path.join(setup_config["output_dir_path"], "xlsx")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for well_name in wells:
        well_data = time_series_df.select("time", well_name)
        metadata = pl.DataFrame(
            {
                "metadata": [
                    well_name,
                    setup_config["recording_date"],
                    setup_config.get("barcode", "N/A"),
                    str(setup_config["fps"]),
                    "y",  # do twitches point up? (y/n)
                    "NAUTILAI",  # instrument serial number
                    None,  # resample period
                    setup_config["data_type"],
                ]
            }
        )

        output_path = os.path.join(output_dir, f"{well_name}.xlsx")
        with Workbook(output_path) as wb:
            well_data.write_excel(wb, "sheet", position="A2", has_header=False)
            metadata.write_excel(wb, "sheet", position="E2", has_header=False)

    with zipfile.ZipFile(os.path.join(setup_config["output_dir_path"], "xlsx-results.zip"), "w") as zf:
        for dir_name, _, file_names in os.walk(output_dir):
            for file_name in file_names:
                file_path = os.path.join(dir_name, file_name)
                zf.write(file_path, os.path.basename(file_path))


if __name__ == "__main__":
    try:
        main()
    except Exception:
        logger.exception("Error in Local Analysis")
        sys.exit(1)
