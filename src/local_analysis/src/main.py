"""Nautilai Local Analysis."""

import argparse
import logging

import toml

from process import process

logging.basicConfig(
    format="[%(asctime)s.%(msecs)03d] [well_data] [%(levelname)s] %(message)s",
    level=logging.INFO,
    datefmt="%Y-%m-%d %H:%M:%S",
)
logger = logging.getLogger(__name__)


def main():
    parser = argparse.ArgumentParser(description="Extracts signals from a multi-well microscope experiment")
    parser.add_argument(
        "toml_config_path", type=str, default=None, help="Path to a toml file with run config parameters"
    )
    parser.add_argument("--input_path", type=str, default=None, help="Path to a video with multi-well data")
    parser.add_argument("--output_dir_path", type=str, default=None, help="Path to save all output")
    parser.add_argument("--num_horizontal_pixels", type=int, default=None, help="Number of horizontal pixels")
    parser.add_argument("--num_vertical_pixels", type=int, default=None, help="Number of vertical pixels")
    parser.add_argument("--num_frames", type=int, default=None, help="Number of frames")
    parser.add_argument("--bit_depth", type=int, default=None, help="number of bits per pixel")
    parser.add_argument(
        "--scale_factor",
        type=int,
        default=None,
        help="Scaling factor, a 3072x2048 image has a scale factor of 1, a 1536x1024 has a scale factor of 2",
    )
    parser.add_argument("--duration", type=float, default=None, help="Duration of recording, in seconds")
    parser.add_argument("--fps", type=float, default=None, help="number of frames per second")

    # TODO add cmd line arg and/or toml config value to point to location of background data

    cmd_line_args = parser.parse_args()

    with open(cmd_line_args.toml_config_path) as toml_file:
        setup_config = toml.load(toml_file)

    setup_config |= {k: v for k, v in vars(cmd_line_args).items() if v is not None}

    # this value is not necessary anymore after loading the config file
    setup_config.pop("toml_config_path")

    process(setup_config)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        logger.exception(f"Error in Local Analysis")
