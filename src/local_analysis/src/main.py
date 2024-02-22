"""Nautilai Local Analysis."""

import argparse
import logging
import sys
import os

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

    # TODO add cmd line args and/or toml config values for the background data inputs

    cmd_line_args = parser.parse_args()

    with open(cmd_line_args.toml_config_path) as toml_file:
        setup_config = toml.load(toml_file)

    logger.info(f"Metadata: {setup_config}")

    process(setup_config)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        logger.exception(f"Error in Local Analysis")
        sys.exit(1)
