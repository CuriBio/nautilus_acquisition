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
from PIL import Image, ImageDraw, ImageFont, ImageTk
import polars as pl
import pyarrow.parquet as pq
from scipy.stats import linregress
import toml
import tkinter as tk
from tkinter import filedialog
import ijroi
import struct


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

class NautilaiApplication:
    def __init__(self, root: tk.Tk) -> None:
        self.root = root
        self.root2 = tk.Toplevel(root)
        self._setup_gui()
        self.setup_config = None
        self.rect = None
        self.start_x = None
        self.start_y = None
        self.stop = None
        self.active_roi = None
        self.roi_index = {}
        self.mode = None
        self.margin = 10
        self.roi_preview.bind("<ButtonPress-1>", self.on_click)
        self.roi_preview.bind("<B1-Motion>", self.on_drag) 
        self.roi_preview.bind("<ButtonRelease-1>", self.on_release)

    def _setup_gui(self) -> None:
        self.file_label = tk.Label(self.root, text="Select settings file:")
        self.file_label.pack()

        self.file_entry = tk.Entry(self.root)
        self.file_entry.pack()

        self.file_button = tk.Button(self.root, text="Load Settings File", command=self._choose_file)
        self.file_button.pack()

        self.roi_set_label = tk.Label(self.root, text="Select ImageJ RoiSet.zip file:")
        self.roi_set_label.pack()

        self.roi_set_entry = tk.Entry(self.root)
        self.roi_set_entry.pack()

        self.roi_set_button = tk.Button(self.root, text="Load ImageJ ROI set", command=self._choose_roi_set)
        self.roi_set_button.pack()

        self.use_background_subtraction_value = tk.BooleanVar()
        self.use_background_subtraction_checkbutton = tk.Checkbutton(
            self.root, text="Use Background Subtraction", variable=self.use_background_subtraction_value, command = self.update_background_subtraction
        )
        self.use_background_subtraction_checkbutton.pack()

        self.run_button = tk.Button(self.root, text="Run", command=self._run_main_function)
        self.run_button.pack()
        
        # self.roi_preview = tk.Canvas(self.root, width=500, height=500, bg="white")
        self.roi_preview = tk.Canvas(self.root2, width=500, height=500, bg="white")
        self.roi_preview.pack( expand = True, fill = tk.BOTH)

        self.scroll_x = tk.Scrollbar(self.roi_preview, orient=tk.HORIZONTAL, command=self.roi_preview.xview)
        self.scroll_x.pack(side=tk.BOTTOM, fill=tk.X)
        self.scroll_y = tk.Scrollbar(self.roi_preview, orient="vertical", command=self.roi_preview.yview)
        self.scroll_y.pack(side="right", fill="y")
        self.roi_preview.config(xscrollcommand=self.scroll_x.set, yscrollcommand=self.scroll_y.set)


       
    def update_background_subtraction(self):
        self.use_background_subtraction_value = not self.use_background_subtraction_value
        self.setup_config["use_background_subtraction"] = self.use_background_subtraction_value
        
    def _choose_file(self) -> dict[str, Any]:
        file_path = filedialog.askopenfilename()
        self.file_entry.delete(0, tk.END)
        self.file_entry.insert(0, file_path)
        with open(self.file_entry.get()) as toml_file:
            self.setup_config = toml.load(toml_file)
        self.setup_config["recording_name"] = os.path.splitext(os.path.basename(self.setup_config["input_path"]))[0]
        _scale_inputs(self.setup_config)
        rois = _create_rois(self.setup_config)
        self.setup_config["rois"] = rois
        self.use_background_subtraction_value = self.setup_config["use_background_subtraction"]
        if self.setup_config["use_background_subtraction"]:
            self.use_background_subtraction_checkbutton.select()
        rois = self.setup_config["rois"]
        _write_ij_rois(rois, self.setup_config)
        self.setup_config["roi_source"] = "Auto"
        # print(self.setup_config)
        self._create_roi_preview()

    def _choose_roi_set(self):
        file_path = filedialog.askopenfilename()
        self.roi_set_entry.delete(0, tk.END)
        self.roi_set_entry.insert(0, file_path)
        if os.path.exists(self.roi_set_entry.get()):
            ij_rois = read_roi_set(self.roi_set_entry.get())
            self.setup_config["rois"] = ij_rois
            self.setup_config["roi_source"] = "Custom"
            self._create_roi_preview()

    def _create_roi_preview(self):
        
        logger.info("Creating ROI sanity check image")
        
        
        setup_config = self.setup_config
        raw_data_reader = _create_raw_data_reader(setup_config)
        rois = setup_config["rois"]

        # second frame is used for the roi annotation
        first_frame = raw_data_reader.frame(1)

        # scale values down to uint8 so PIL can process them
        if setup_config["bit_depth"] in (12, 16):
            first_frame = (first_frame / first_frame.max() * 255).astype("uint8")

        # TODO why is this necessary? Assuming it's for the colors
        first_frame = np.stack((first_frame, first_frame, first_frame), axis=-1)

        # for well_name, roi in rois.items():
        #     cv.rectangle(
        #         img=first_frame,
        #         pt1=dataclasses.astuple(roi.p_ul),
        #         pt2=dataclasses.astuple(roi.p_br),
        #         color=(0, 255, 0),  # gbr
        #         thickness=1,
        #         lineType=cv.LINE_AA,
        #     )

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
        
        tk_image = ImageTk.PhotoImage(first_frame_image)
        
        # roi_preview = tk.Label(root, image = tk_image)
        # roi_preview.config(image = tk_image)
        # self.roi_preview.destroy()
        # self.roi_preview = tk.Canvas(self.root, width=500, height=500, bg="white")
        # self.roi_preview.pack()
        self.roi_preview.delete("all")
        self.roi_preview.config(width = first_frame_image.width, height = first_frame_image.height)
        self.roi_preview.create_image(0, 0, image = tk_image, anchor = tk.NW)
        self.roi_preview.image = tk_image
        
        # self.rect = self.roi_preview.create_rectangle(10, 10, 100, 100, outline="red")
        for well_name, roi in rois.items():
            print(roi.p_ul.x, roi.p_ul.y, roi.p_br.x, roi.p_br.y)
            self.rect = self.roi_preview.create_rectangle(roi.p_ul.x, roi.p_ul.y, roi.p_br.x, roi.p_br.y, outline="red")
            self.roi_index[well_name] = self.rect
        return(first_frame_image)
    
    def on_click(self, event):
        self.start_x = event.x
        self.start_y = event.y
        for well_name, roi in self.setup_config["rois"].items():
            x1, y1, x2, y2 = roi.p_ul.x, roi.p_ul.y, roi.p_br.x, roi.p_br.y
            if x1 <= event.x <= x2 and y1 <= event.y <= y2:
                self.active_roi = well_name
                if abs(x1 - event.x) < self.margin:
                    self.mode = "resize_left"
                    if abs(y1 - event.y) < self.margin:
                        self.mode = "resize_topleft"
                        return None
                    if abs(y2 - event.y) < self.margin:
                        self.mode = "resize_bottomleft"
                        return None
                    return None
                if abs(x2 - event.x) < self.margin:
                    self.mode = "resize_right"
                    if abs(y1 - event.y) < self.margin:
                        self.mode = "resize_topright"
                        return None
                    if abs(y2 - event.y) < self.margin:
                        self.mode = "resize_bottomright"
                        return None
                    return None
                if abs(y1 - event.y) < self.margin:
                    self.mode = "resize_top"
                    return None
                if abs(y2 - event.y) < self.margin:
                    self.mode = "resize_bottom"
                    return None
                self.mode = "move"
                return None
        # print(self.rect)
        # print(self.roi_preview.coords(2))
        # if (self.rect is None):
        #     self.start_x = event.x
        #     self.start_y = event.y
        #     self.rect = self.roi_preview.create_rectangle(self.start_x, self.start_y, self.start_x, self.start_y, outline="red")
        # else:
        #     x1, y1, x2, y2 = self.roi_preview.coords(self.rect)
        #     if x1 <= event.x <= x2 and y1 <= event.y <= y2:
        #         self.start_x = event.x
        #         self.start_y = event.y
        #     else:
        #         self.start_x = event.x
        #         self.start_y = event.y
        #         self.rect = self.roi_preview.create_rectangle(self.start_x, self.start_y, self.start_x, self.start_y, outline="red")
    def on_drag(self, event):
        if self.active_roi is not None:
            if self.mode == "move":
                self.roi_preview.move(self.roi_index[self.active_roi], event.x - self.start_x, event.y - self.start_y)
                self.start_x = event.x
                self.start_y = event.y
                return None
            if self.mode == "resize_left":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], event.x, y1, x2, y2)
                return None
            if self.mode == "resize_right":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], x1, y1, event.x, y2)
                return None
            if self.mode == "resize_top":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], x1, event.y, x2, y2)
                return None
            if self.mode == "resize_bottom":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], x1, y1, x2, event.y)
                return None
            if self.mode == "resize_topleft":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], event.x, event.y, x2, y2)
                return None
            if self.mode == "resize_bottomleft":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], event.x, y1, x2, event.y)
                return None
            if self.mode == "resize_topright":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], x1, event.y, event.x, y2)
                return None
            if self.mode == "resize_bottomright":
                x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
                self.roi_preview.coords(self.roi_index[self.active_roi], x1, y1, event.x, event.y)
                return None
        # if (self.stop is None):
        #     self.stop_x = event.x
        #     self.stop_y = event.y
        #     self.roi_preview.coords(self.rect, self.start_x, self.start_y, self.stop_x, self.stop_y)
        # else:
        #     x1, y1, x2, y2 = self.roi_preview.coords(self.rect)
        #     if x1 <= event.x <= x2 and y1 <= event.y <= y2:
        #         self.roi_preview.move(self.rect, event.x - self.start_x, event.y - self.start_y)
        #         self.start_x = event.x
        #         self.start_y = event.y
        #     else:
        #         self.stop_x = event.x
        #         self.stop_y = event.y
        #         self.roi_preview.coords(self.rect, self.start_x, self.start_y, self.stop_x, self.stop_y)
    def on_release(self, event):
        x1, y1, x2, y2 = self.roi_preview.coords(self.roi_index[self.active_roi])
        self.setup_config["rois"][self.active_roi] = RoiCoords(Point(int(x1),int(y1)),Point(int(x2),int(y2)))
        print("well name",self.active_roi)
        print("Roi value",self.setup_config["rois"][self.active_roi])
        print("canvas index",self.roi_index[self.active_roi])
        print("canvas coordinates",self.roi_preview.coords(self.roi_index[self.active_roi]))
        self.stop = event.x, event.y
        self.active_roi = None
        _write_ij_rois(self.setup_config["rois"], self.setup_config)
        self.setup_config["roi_source"] = "Custom"
    def _run_main_function(self):
        main(self.root, self.setup_config)

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


def main(root:tk.Tk, setup_config: dict) -> None:
    logging.basicConfig(
        format="[%(asctime)s.%(msecs)03d] [local_analysis] [%(levelname)s] %(message)s",
        level=logging.INFO,
        datefmt="%Y-%m-%d %H:%M:%S",
        stream=sys.stdout
    )

    logger.info("Nautilai Local Analysis Starting")

    #toml_config_path = file_entry.get()
  
    

    #parse toml_config_path from command line arguments
    #parser = ArgParse(description="Extracts signals from a multi-well microscope experiment")
    #parser.add_argument(
    #    "toml_config_path", type=str, default=None, help="Path to a toml file with run config parameters"
    #)
    # cmd_line_args = parser.parse_args()


    # if os.path.exists(app.roi_set_entry.get()):
    #     ij_rois = read_roi_set(app.roi_set_entry.get())
    
    # setup_config["use_background_subtraction"] = app.use_background_subtraction_value.get()

    logger.info(f"Metadata: {setup_config}")

    raw_data_reader = _create_raw_data_reader(setup_config)
    rois = setup_config["rois"]
    # _write_ij_rois(rois, setup_config)
    # setup_config["roi_source"] = "Auto"
    # if os.path.exists(app.roi_set_entry.get()):
    #     rois = ij_rois
    #     setup_config["roi_source"] = "Custom"

    _create_roi_annotated_image(raw_data_reader, setup_config, rois)

    time_series_df = _calculate_fluorescence_time_series(raw_data_reader, rois, setup_config)

    if setup_config["use_background_subtraction"]:
        if plate_id := setup_config["plate_id"]:
            background_recording_info = _load_background(setup_config["background_recording_dir"], plate_id)
            time_series_df = _subtract_background(time_series_df, setup_config, background_recording_info)
        else:
            logger.info("Background subtraction enabled, however no Plate ID was given")
    else:
        logger.info("Background subtraction disabled")

    _write_time_series_parquet(time_series_df, setup_config)
    _write_time_series_csv(time_series_df, setup_config)
    _create_time_series_plot_image(time_series_df, setup_config)
    _write_time_series_legacy_xlsx_zip(time_series_df, setup_config)

    logger.info("Done")


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

#read the roi set from the ImageJ roi zip file
def read_roi_set(roi_path) -> dict[str, RoiCoords]:
    try:
        roi_set = ijroi.read_roi_zip(roi_path)
        pass
    except Exception as e:
        print(f"An error occurred: {e}")
        return None
    rois = {}
    for roi_set_item in roi_set:
        try:
            rois[roi_set_item[0].replace(".roi", "")] = RoiCoords(
                        Point(
                            int(roi_set_item[1][0][1]),
                            int(roi_set_item[1][0][0]),
                        ),
                        Point(
                            int(roi_set_item[1][2][1]),
                            int(roi_set_item[1][2][0]),
                        ),
                    )
            pass
        except Exception as e:
            print(f"An error occurred: {e}")
            return None
    return rois




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
    # print(rois)
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


def _load_background(bg_recording_dir: str, plate_id: str) -> BackgroundRecordingInfo:
    logger.info("Loading background recording data")

    if not os.path.exists(bg_recording_dir):
        raise Exception(f"Background recording dir ({bg_recording_dir}) does not exist")

    plate_id_dir_path = os.path.join(bg_recording_dir, plate_id)
    if not os.path.exists(plate_id_dir_path):
        raise Exception(f"Background recording dir ({plate_id_dir_path}) for plate ID not found")
    
    bg_rec_data_path = os.path.join(plate_id_dir_path, f"{plate_id}.tsv")
    if not os.path.exists(bg_rec_data_path):
        raise Exception(f"Background recording file ({bg_rec_data_path}) not found")

    bg_rec_data = pl.read_csv(bg_rec_data_path, has_header=True, separator="\t")

    bg_rec_metadata_path = os.path.join(plate_id_dir_path, "settings.toml")
    with open(bg_rec_metadata_path) as toml_file:
        bg_rec_metadata = toml.load(toml_file)

    return BackgroundRecordingInfo(data=bg_rec_data, metadata=bg_rec_metadata)


def _subtract_background(
    time_series_df: pl.DataFrame, setup_config: dict[str, Any], bg_recording_info: BackgroundRecordingInfo
) -> pl.DataFrame:
    logger.info("Performing background subtraction")

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

    barcode_field = "N/A"
    if setup_config["use_background_subtraction"] and (plate_id := setup_config["plate_id"]):
        barcode_field = plate_id

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
        "roi_source": setup_config["roi_source"],
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
                    setup_config["roi_source"],
                ]
            }
        )

        output_path = os.path.join(output_dir, f"{well_name}.xlsx")
        with Workbook(output_path) as wb:
            well_data.write_excel(wb, "sheet", position="A2", include_header=False)
            metadata.write_excel(wb, "sheet", position="E2", include_header=False)

    with zipfile.ZipFile(os.path.join(setup_config["output_dir_path"], "xlsx-results.zip"), "w") as zf:
        for dir_name, _, file_names in os.walk(output_dir):
            for file_name in file_names:
                file_path = os.path.join(dir_name, file_name)
                zf.write(file_path, os.path.basename(file_path))

def _write_ij_rois(rois: dict[str, RoiCoords], setup_config: dict):
    output_dir = os.path.join(setup_config["output_dir_path"], "rois")
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    for well_name, roi in rois.items():
        output_path = os.path.join(output_dir, f"{well_name}.roi")
        #write ImageJ roi binary file, see https://imagej.net/ij/developer/api/ij/ij/io/RoiDecoder.html
        with open(output_path, 'wb') as binary_file:
            binary_file.write('Iout'.encode('utf-8')) #0-3             "Iout"
            binary_file.write(struct.pack('>H', 228)) #4-5             version (>=217)
            binary_file.write(struct.pack('B', 1)) #6            roi type (encoded as one byte)
            binary_file.write(struct.pack('B', 0)) #7 empty
            binary_file.write(struct.pack('>H', roi.p_ul.y)) #8-9             top
            binary_file.write(struct.pack('>H', roi.p_ul.x)) #10-11   left
            binary_file.write(struct.pack('>H', roi.p_br.y)) # 12-13   bottom
            binary_file.write(struct.pack('>H', roi.p_br.x)) #14-15   right
            binary_file.write(struct.pack('>H', 0)) #16-17   NCoordinates
            #18-33   x1,y1,x2,y2 (straight line) | x,y,width,height (double rect) | size (npoints)
            binary_file.write(struct.pack('f', 0.0)) #18-21 x1, float 32
            binary_file.write(struct.pack('f', 0.0)) #22-25 y1, float 32
            binary_file.write(struct.pack('f', 0.0)) #26-29 x2, float 32
            binary_file.write(struct.pack('f', 0.0)) #30-33 y2, float 32
            binary_file.write(struct.pack('>H', 0)) #34-35   stroke width (v1.43i or later)
            binary_file.write(struct.pack('i', 0)) #36-39   ShapeRoi size (type must be 1 if this value>0)
            binary_file.write(struct.pack('I', 16777215)) #40-43   stroke color (v1.43i or later)
            binary_file.write(struct.pack('i', 0)) #44-47   fill color (v1.43i or later)
            binary_file.write(struct.pack('>H', 0)) #48-49   subtype (v1.43k or later)
            binary_file.write(struct.pack('>H', 0)) #50-51   options (v1.43k or later)
            binary_file.write(struct.pack('B', 0)) #52-52   arrow style or aspect ratio (v1.43p or later)
            binary_file.write(struct.pack('B', 0)) #53-53   arrow head size (v1.43p or later)
            binary_file.write(struct.pack('>H', 0)) #rounded rect arc size (v1.43p or later)
            binary_file.write(struct.pack('i', -1509031936)) #56-59   position
            binary_file.write(struct.pack('i', 1073741824)) #60-63   header2 offset
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 2147483648)) #magic
            binary_file.write(struct.pack('I', 33554432)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write(struct.pack('I', 0)) #magic
            binary_file.write('          '.encode('utf-8')) #"magic number"
            binary_file.write(well_name.encode('utf-8')) #"magic number"
    with zipfile.ZipFile(os.path.join(setup_config["output_dir_path"], "DefaultRoiSet.zip"), "w") as zf:
        for dir_name, _, file_names in os.walk(output_dir):
            for file_name in file_names:
                file_path = os.path.join(dir_name, file_name)
                zf.write(file_path, os.path.basename(file_path))

def _create_roi_preview(
    setup_config: dict[str, Any]
) -> None:
    logger.info("Creating ROI sanity check image")
    setup_config["recording_name"] = os.path.splitext(os.path.basename(setup_config["input_path"]))[0]
    _scale_inputs(setup_config)
    raw_data_reader = _create_raw_data_reader(setup_config)
    rois = _create_rois(setup_config)

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
    
    tk_image = ImageTk.PhotoImage(first_frame_image)
    # roi_preview = tk.Label(root, image = tk_image)
    # roi_preview.config(image = tk_image)
    
    app.config(width = first_frame_image.width, height = first_frame_image.height)
    app.roi_preview.create_image(0, 0, image = tk_image, anchor = tk.NW)
    app.roi_preview.image = tk_image
    return(first_frame_image)



if __name__ == "__main__":
    try:
        root = tk.Tk()
        root.title("Nautilai Local Analysis Tool")
        app = NautilaiApplication(root)        
        root.mainloop()
        #main()
    except Exception:
        logger.exception("Error in Local Analysis")
        sys.exit(1)
