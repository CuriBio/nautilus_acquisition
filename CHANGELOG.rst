Changelog for Nautilus
======================
0.1.19 (unreleased)
-------------------

Added:
^^^^^^
- Drop down menu that can be used to select plate format from selected file


0.1.18 (2023-04-07)
-------------------

Added:
^^^^^^
- avi output for auto tiling if `encode_video` is set to true in config file
- stage connection error modal
- acquisition progress bar

Fixed:
^^^^^^
- Refactored config parsing


0.1.17 (2023-03-31)
-------------------

Fixed:
^^^^^^
- Drive space check calculation didn't use number of stage positions
- UI wasn't updating properly for drive space check when changing values


0.1.16 (2023-03-31)
-------------------

Added:
^^^^^^
- Add stage calibration/measure, xy pixel size in config, stage step sizes in config


0.1.15 (2023-03-30)
-------------------

Added:
^^^^^^
- Max frame rate is calculated based on capture mode and height of campture area. Users can not select a frame rate higher than the calculated max.
- Auto tiling support


0.1.14 (2023-03-14)
-------------------

Fixed:
^^^^^^
- Scale width/height by respective binning factor


0.1.13 (2023-03-10)
-------------------

Fixed:
^^^^^^
- Moved output directory check from frame acquistion thread to updateExp method so it's not checking the output directory on every frame callback
- Change available_space_in_default_drive method so it is defined for non-win32 systems
- Fix settings output, switch to toml output

Added:
^^^^^^
- Check led intensity is > 0.0 before turning on led
- Switch default output to TiffStack + BigTiff
- Add horizontal/vertical live view image flipping to config
- Increase default binning factor


0.1.12 (2023-02-27)
-------------------

Fixed:
^^^^^^
- Error related to stage controls not working in Stage Navigation modal

0.1.11 (2023-02-17)
-------------------

Added:
^^^^^^
- Ui option in advanced setup to select name of ni device


0.1.10 (2023-02-16)
-------------------

Added:
^^^^^^
- Automatic saving of stage position list on shutdown and loading of previous stage position list on launch

Fixed:
^^^^^^
- Errors on shutdown


0.1.9 (2023-02-16)
------------------

Added:
^^^^^^
- Store TIFF files under subdirectory in the format prefix__YYYY_M_D_HMS
- Add settings.txt file to each acquisition with led intensity, stage positions, duration, and frame rate


0.1.8 (2023-02-16)
------------------

Changed:
^^^^^^^^
- Stage controls have been moved to ``Stage Navigation`` modal


0.1.7 (2023-02-14)
------------------

Added:
^^^^^^
- Check before each acquisition for necessary space in default drive


0.1.6 (2023-02-14)
------------------

Changed:
^^^^^^^^
- Only allocate memory for acquisition once, and only set up exposure when starting acquisition


0.1.5 (2023-02-14)
------------------

Changed:
^^^^^^^^
- If no stage positions set when acquisition begins, use current state position


0.1.4 (2023-02-13)
------------------

Added:
^^^^^^
- Default drive used for acquisition data can be set with nautilus.toml outdir property


0.1.3 (2023-02-10)
------------------

Changed:
^^^^^^^^
- Disabled frame rate edit during live view
