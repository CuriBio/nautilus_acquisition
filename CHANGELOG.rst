Changelog for Nautilai
0.2.2 (2024-08-20)
------------------

Added:
^^^^^^
- Extra logging for WriteFile call


0.2.1 (2024-08-13)
------------------

Added:
^^^^^^
- Extra logging for win api calls


0.2.0 (2024-08-05)
------------------

Added:
^^^^^^
- Background recording


0.1.46 (2024-07-16)
-------------------

Fixed:
^^^^^^
- App crashing on startup when sbin and pbin are set to 4


0.1.45 (2024-07-12)
-------------------

Added:
^^^^^^
- Local Analysis logs are now captured in the controller's logs
- Progress bar is now shown during background recordings

Fixed:
^^^^^^
- Acquisition failing to run after changing recording location in Save Options
- Handling of errors related to nautilai.toml and machine.toml files
- Turn off LEDs after each capture during background recording
- Installer now works for multiple users
- Update check now runs in background instead of blocking during stage calibration
- Background subtraction now runs correctly in Local Analysis


0.1.44 (2024-06-04)
-------------------

Changed:
^^^^^^^^
- Removed unused "File Type" drop down in save options

Fixed:
^^^^^^
- Fix pyinstaller version


0.1.43 (2024-05-17)
-------------------

Added:
^^^^^^
- Background subtraction to Local Analysis
- Plate Barcode field in metadata of Local Analysis output will now be set to the plate ID if background subtraction is performed
- Added background recordings

Changed:
^^^^^^^^
- Log files are now located in C:\Users\<user>\Documents\Nautilai\Logs
- Platemap preview now supports all 24 and 96 well plates
- Local Analysis now runs after video encoding failure
- Cleanup compiler warning
- Enable avx2 compile option
- Clear frame queue when stopping acquisition no images are captured
- Update platemaps
- Max length for file prefix is now 200 chars

Fixed:
^^^^^^
- Invalid chars are no longer allowed in file prefix


Fixed:
^^^^^^
- Advanced Settings now shows the correct values after changes are made


0.1.42 (2024-04-10)
-------------------

Added:
^^^^^^
- Local Analysis outputs xlsx format again
- Option to specify AVI video quality in advanced setup

Changed:
^^^^^^^^
- Stage Navigation menu:

  - Removed add, remove, save, and load buttons
  - Added skip and unskip buttons which allow for skipping a stage position during acquisition
  - Cleaned up styling

- Plate format definitions

Fixed:
^^^^^^
- frameCount narrowed to uint16_t in AutoTile call, overflows when number of frames above 65536, changed to uint32_t


0.1.41 (2024-03-11)
-------------------

Added:
^^^^^^
- Show warning when frate rate is <= 1 Hz
- updated nautilai.toml file
- autoupdate functionality
- switched installer to per-user location
- removed machine_vars_path from nautilai.toml and hard-coded to C:\ProgramData\Curi Bio\Nautilai\machine.toml
- Plate ID widget

Changed:
^^^^^^^^
- Nautilai rename
- Removal of default config values
- Local Analysis now outputs time series data as parquet
- UI layout

Fixed:
^^^^^^
- Machine-specific variable requirement


0.1.40 (2024-01-10)
-------------------

Fixed:
^^^^^^
- Manual trigger functionality


0.1.39 (2023-12-11)
-------------------

Fixed:
^^^^^^
- Image artifact issue


0.1.38 (2023-09-27)
-------------------

Added:
^^^^^^
- Advanced setup option to downsample raw files by user-selected bin factor and delete original raw file
- User ability to send manual trigger during acquisition when trigger mode is set to EXT_TRIG_TRIG_FIRST

Changed:
^^^^^^^^
- Set shader thresholds to display blue if pixel is < 1.0 and red if pixel is >= max threshold value
- Updated acquisition region settings in nautilus.toml

Fixed:
^^^^^^
- NI device selection in advanced setup dialog
- Only save advanced setup options when user confirms and not on dialog Close
- Select platemap option after selecting ``Cancel`` in no platemap warning dialog
- Fixed regression in window resizing/aspect ratio


0.1.37 (2023-09-12)
-------------------

Added:
^^^^^^
- Check limiting the length of recording directory + file prefix path
- Ability to change trigger mode inside the Advanced Settings menu
- Ability to specify the type of data being recorded. Current options are ``Calcium Imaging``
  and ``Voltage Imaging``
- Ability to disable Live View while acquisition is running
- Warn users when no platemap has been selected when starting acquisition

Changed:
^^^^^^^^
- Turn off live view and LED light when acquisition stops
- Update plate format files

Fixed:
^^^^^^
- HiLo look up table max is now 4095
- Use of platemap SVGs with green well active indicators
- Enable Stage Navigation buttons after selection while Live View is running


0.1.36 (2023-07-31)
-------------------

Added:
^^^^^^
- UI widget masks
- Async stage dialog control do ui isn't blocked when moving stage


0.1.35 (2023-07-27)
-------------------

Added:
^^^^^^
- Move live view auto level calculation to shader pipeline
- Add brand style


0.1.34 (2023-07-24)
-------------------

Added:
^^^^^^
- Set trigger mode to internal trigger when starting live view only
- Change trigger to value in config when starting acquisition
- Fully stop acquisition after stage move before restarting for next region
- Disable exit when post processing
- Prevent line-wrapping in settings.toml for long paths
- Error on non-E drive selection


0.1.33 (2023-07-12)
-------------------

Fixed:
^^^^^^
- Updated UI state handling
- Changed signal to manual reset to fix WriteFile hanging issue
- Changed so live view can run between image acquisitions
- Output frame queue size every 1000 frames


0.1.31 (2023-06-01)
-------------------

Fixed:
^^^^^^
- Updated recording date fmt string to match what p3d expects


0.1.30 (2023-05-26)
-------------------

Added:
^^^^^^
- Added check for already running process


0.1.29 (2023-05-26)
-------------------

Added:
^^^^^^
- Exp backoff for starting video encoding


0.1.28 (2023-05-25)
-------------------

Added:
^^^^^^
- Changed ffmpeg options to gray16le pix fmt, output avi file

Fixed:
^^^^^^
- Advanced settings dialog crash
- Close raw file before video encoding


0.1.27 (2023-05-23)
-------------------

Added:
^^^^^^
- Call external ffmpeg bin for video encoding
- StorageType::Raw option writes raw files with overlapped i/o
- Autotile raw files


0.1.26 (2023-05-18)
-------------------

Added:
^^^^^^
- Delete files when autotiling
- Update plate format files
- Use thread pool for writing files in parallel
- Allow frame pool size to 90% max memory, defaults to frameCount
- ensure pool size of frameCount at acquisition start
- enable async init by default
- set busy wait cursor during initialize
- move all init code to thread and disable ui until init is finished,
  prevents ui from blocking

Fixed:
^^^^^^
- Crash when stage positions is less than rows*cols
- Update camera->curExp when fps is changed


0.1.25 (2023-05-04)
-------------------

Added:
^^^^^^
- Tile map config value
- Auto flush logger every 10 seconds
- Signals for starting/stopping external analysis
- Set stage units to um

Fixed:
^^^^^^
- Wrong tiling order on second row


0.1.24 (2023-04-27)
-------------------

Added:
^^^^^^
- Added well_data.exe to resources
- Write out settings file with platemap/wellmap info
- Call well_data.exe after post-processing images


0.1.23 (2023-04-26)
-------------------

Added:
^^^^^^
- led ON shutter delay

Fixed:
^^^^^^
- hflip chunk offset index
- Liveview 16bit half view with
- Prefix change crash


0.1.22 (2023-04-24)
-------------------

Fixed:
^^^^^^
- Issues with liveview, cancelation and progress bar show hints
- Uninitialized data for liveview when running without auto contrast/brightness


0.1.21 (2023-04-24)
-------------------

Added:
^^^^^^
- Added new platemap format files to resources

Fixed:
^^^^^^
- Half frame video when auto contrast/brightness is disabled and video is in 16bit mode


0.1.20 (2023-04-21)
-------------------

Added:
^^^^^^
- Encode video when auto_contrast_brightness is false
- Fix crash with autotile false/rows*cols mismatch


0.1.19 (2023-04-20)
-------------------

Added:
^^^^^^
- Raw file support
- Turn led off before auto tiling
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
- Moved output directory check from frame acquisition thread to updateExp method so it's not checking the output directory on every frame callback
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
