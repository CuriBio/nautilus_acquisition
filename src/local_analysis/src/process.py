"""Processing for Nautilai Local Analysis."""

import os
from typing import Any
import logging

import numpy as np
from PIL import Image, ImageDraw, ImageFont

logger = logging.getLogger(__name__)

from dataclasses import dataclass


@dataclass
class Roi:
    well_name: str
    x: int
    y: int


def process(setup_config: dict[str, Any]):
    _scale_inputs(setup_config)

    raw_data = _load_raw_data(setup_config)

    _create_rois(setup_config)

    _create_roi_annotated_image(raw_data, setup_config)

    # _calculate_fluorescence_time_series()

    # if background_data_path := setup_config["background_data_path"]:
    #     _load_background(background_data_path)
    #     _subtract_background()

    # _create_time_series_plot_image()


def _scale_inputs(setup_config: dict[str, Any]):
    setup_config["xy_pixel_size"] *= setup_config["additional_bin_factor"]

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
    pass


def _create_roi_annotated_image(raw_data: np.ndarray, setup_config: dict[str, Any]):
    first_frame_copy = np.copy(raw_data[10])

    # scale values down to uint8 so PIL can process them
    if setup_config["bit_depth"] in (12, 16):
        first_frame_copy = (first_frame_copy / first_frame_copy.max() * 255).astype("uint8")

    # first_frame_copy = np.stack((first_frame_copy, first_frame_copy, first_frame_copy), axis=-1)

    first_frame_image = Image.fromarray(first_frame_copy)

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
