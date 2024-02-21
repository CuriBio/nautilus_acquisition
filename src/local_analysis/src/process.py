"""Processing for Nautilai Local Analysis."""

from typing import Any


def process(setup_config: dict[str, Any]):
    _load_raw_data()
    _load_rois()

    _calculate_fluorescence_time_series()

    if background_data_path := setup_config["background_data_path"]:
        _load_background(background_data_path)
        _subtract_background()

    _create_roi_annotated_image()
    _create_time_series_plot_image()


def _load_raw_data():
    pass


def _load_rois():
    pass


def _calculate_fluorescence_time_series():
    pass


def _load_background():
    pass


def _subtract_background():
    pass


def _create_roi_annotated_image():
    pass


def _create_time_series_plot_image():
    pass
