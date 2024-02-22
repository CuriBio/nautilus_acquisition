"""Processing for Nautilai Local Analysis."""

import os
from typing import Any
import logging
import cv2 as cv

import numpy as np
from PIL import Image, ImageDraw, ImageFont

logger = logging.getLogger(__name__)

import dataclasses
from dataclasses import dataclass


def _get_well_row_name(row: int):
    well_name = chr(ord("A") + row % 26)
    if row >= 26:
        well_name = "A" + well_name
    return well_name


MAX_ROWS = 32
ALL_WELL_ROWS = tuple([_get_well_row_name(row) for row in range(MAX_ROWS)])


@dataclass
class Point:
    x: int
    y: int


@dataclass
class RoiCoords:
    p_ul: Point  # upper left
    p_br: Point  # bottom right


def process(setup_config: dict[str, Any]):
    _scale_inputs(setup_config)

    raw_data = _load_raw_data(setup_config)

    rois = _create_rois(setup_config)

    _create_roi_annotated_image(raw_data, setup_config, rois)

    # _calculate_fluorescence_time_series()

    # if background_data_path := setup_config["background_data_path"]:
    #     _load_background(background_data_path)
    #     _subtract_background()

    # _create_time_series_plot_image()


def _scale_inputs(setup_config: dict[str, Any]):
    setup_config["xy_pixel_size"] *= setup_config["additional_bin_factor"] * setup_config["scale_factor"]

    setup_config["height"] //= setup_config["additional_bin_factor"]
    setup_config["width"] //= setup_config["additional_bin_factor"]
    setup_config["num_horizontal_pixels"] //= setup_config["additional_bin_factor"]
    setup_config["num_vertical_pixels"] //= setup_config["additional_bin_factor"]
    setup_config["stage"]["roi_size_x"] //= setup_config["additional_bin_factor"]
    setup_config["stage"]["roi_size_y"] //= setup_config["additional_bin_factor"]

    setup_config["stage"]["roi_size_x"] //= setup_config["scale_factor"]
    setup_config["stage"]["roi_size_y"] //= setup_config["scale_factor"]
    setup_config["stage"]["h_offset"] //= setup_config["scale_factor"]
    setup_config["stage"]["v_offset"] //= setup_config["scale_factor"]


def _load_raw_data(setup_config: dict[str, Any]) -> np.ndarray:
    logger.info("Loading raw data")

    if setup_config["bit_depth"] == 8:
        dtype = np.uint8
    else:  # 12, 16
        dtype = np.dtype("<u2")

    # TODO can't load the entire file into memory all at once
    raw_data = np.fromfile(
        file=setup_config["input_path"],
        dtype=dtype,
        count=(
            setup_config["num_frames"]
            * setup_config["num_vertical_pixels"]
            * setup_config["num_horizontal_pixels"]
        ),
    )

    return raw_data.reshape(
        setup_config["num_frames"], setup_config["num_vertical_pixels"], setup_config["num_horizontal_pixels"]
    )


def _create_rois(setup_config: dict[str, Any]):
    pixel_size = setup_config["xy_pixel_size"]
    roi_size_x = setup_config["stage"]["roi_size_x"]
    roi_size_y = setup_config["stage"]["roi_size_y"]
    num_wells_h = setup_config["stage"]["num_wells_h"]
    num_wells_v = setup_config["stage"]["num_wells_v"]
    well_spacing = setup_config["stage"]["well_spacing"]

    rois = {}

    # TODO clean this up
    for tile_v_idx in range(setup_config["rows"]):
        plate_well_row_offset = tile_v_idx * num_wells_v
        for tile_h_idx in range(setup_config["cols"]):
            plate_well_col_offset = tile_h_idx * num_wells_h
            for well_v_idx in range(num_wells_v):
                for well_h_idx in range(num_wells_h):
                    well_x_center = (
                        setup_config["width"] / 2
                        - ((num_wells_h - 1) / 2 - well_h_idx) * (well_spacing / pixel_size)
                        + setup_config["stage"]["h_offset"]
                        + tile_h_idx * (setup_config["width"])
                    )
                    well_y_center = (
                        setup_config["height"] / 2
                        - ((num_wells_v - 1) / 2 - well_v_idx) * (well_spacing / pixel_size)
                        + setup_config["stage"]["vOffset"]
                        + tile_v_idx * (setup_config["height"])
                    )

                    well_name = ALL_WELL_ROWS[well_v_idx + plate_well_row_offset] + str(
                        well_h_idx + plate_well_col_offset + 1
                    )

                    rois[well_name] = RoiCoords(
                        Point(int(well_x_center - roi_size_x / 2), int(well_y_center - roi_size_y / 2)),
                        Point(int(well_x_center + roi_size_x / 2), int(well_y_center + roi_size_y / 2)),
                    )

                    # wellRows.append(well_v_idx + tile_v_idx * num_wells_v)
                    # wellColumns.append(well_h_idx + 1 + tile_h_idx * num_wells_h)

    return rois


def _create_roi_annotated_image(raw_data: np.ndarray, setup_config: dict[str, Any], rois: list[RoiCoords]):
    first_frame_copy = np.copy(raw_data[0])

    # scale values down to uint8 so PIL can process them
    if setup_config["bit_depth"] in (12, 16):
        first_frame_copy = (first_frame_copy / first_frame_copy.max() * 255).astype("uint8")

    first_frame_copy = np.stack((first_frame_copy, first_frame_copy, first_frame_copy), axis=-1)

    for well_name, roi in rois.items():
        cv.rectangle(
            img=first_frame_copy,
            pt1=dataclasses.astuple(roi.p_ul),
            pt2=dataclasses.astuple(roi.p_br),
            color=(0, 255, 0),  # gbr
            thickness=1,
            lineType=cv.LINE_AA,
        )

    first_frame_image = Image.fromarray(first_frame_copy)
    draw = ImageDraw.Draw(first_frame_image)

    font_size_px = 15
    try:
        font = ImageFont.truetype("arial.ttf", font_size_px)
    except OSError:
        font = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", font_size_px)

    text_offset_px = 3
    for well_name, roi in rois.items():
        position = (roi.p_ul.x + text_offset_px, roi.p_br.y - (font_size_px + text_offset_px))
        draw.text(position, well_name, fill="rgb(0, 255, 0)", font=font)

    roi_output_path = os.path.join(setup_config["output_dir_path"], "roi_locations.png")
    first_frame_image.save(roi_output_path)


def _calculate_fluorescence_time_series():
    pass


def _load_background():
    pass


def _subtract_background():
    pass


def _create_time_series_plot_image():
    pass
