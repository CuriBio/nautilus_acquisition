## Prerequisites
* Install PVCAM drivers, https://downloads.curibio.com/software/nautilus/PVCam_3.9.10.5-PMQI_Release_Setup.zip
* Install NiDAQmx drivers, https://downloads.curibio.com/software/nautilus/ni-daqmx_22.8_online.exe
* Install nautilus software, [downloads.curibio.com/software/nautilus/Nautilus-0.1.1-win64.exe](http://downloads.curibio.com/software/nautilus/Nautilus-0.1.1-win64.exe)

The Nautilus installer by default will install the application and create a start menu shortcut. To start the application find in start menu.  

## Configuration
On first run the application will create a new config file at `C:\Users\<your user>\AppData\Local\Nautilus\nautilus.toml` that contains the following settings.   

```
[nautilus]
prefix = "default_"
auto_contrast_brightness = true

[device.nidaqmx]
device = "Dev2"
max_voltage = 1.4

[device.photometrics]
trigger_mode = 0
exposure_mode = 5
speed_table_index = 0

[device.tango]
com = "COM3"

[acquisition]
fps = 10.0
duration = 1.0
led_intensity = 50.0

[acquisition.region]
s1 = 800
p1 = 1000
s2 = 2399
p2 = 2199
sbin = 1
pbin = 1
```

`[nautilus] settings`  
`prefix` is the value used as a prefix for each saved image. For example, the final image name using the default value will be as follows 
`default_N_XXX.tiff`  
where `N` is a 1-based index of the stage position and `XXX` is the frame number  

`auto_contrast_brightness` Controls if auto contrast/brightness is applied to the live view image. This does not affect the image that is saved to disk.  

`[device.nidaqmx] settings`  

`device` The name of the nidaqmx device used to control the LED panel. You must create a new device with the National Instruments software and set this value in order for the LED controller to work.  

`max_voltage` The max voltage allowed for the analog output channel.  

`[device.photometrics] settings`  
`trigger_mode` The camera trigger mode is an enum value for the following modes.  
```
0: EXT_TRIG_INTERNAL
1: EXT_TRIG_TRIG_FIRST
2: EXT_TRIG_EDGE_RISING
3: EXT_TRIG_LEVEL
4: EXT_TRIG_SOFTWARE_FIRST
5: EXT_TRIG_SOFTWARE_EDGE
6: EXT_TRIG_LEVEL_OVERLAP
7: EXT_TRIG_LEVEL_PULSED
```
Refer to the PVCAM documentation for an explanation of each mode.  

`exposure_mode` The camera exposure mode is an enum value for the following modes.  
```
0: EXPOSE_OUT_FIRST_ROW
1: EXPOSE_OUT_ALL_ROWS
2: EXPOSE_OUT_ANY_ROW
3: EXPOSE_OUT_ROLLING_SHUTTER
4: EXPOSE_OUT_LINE_TRIGGER
5: EXPOSE_OUT_GLOBAL_SHUTTER
6: MAX_EXPOSE_OUT_MODE
```
Refer to the PVCAM documentation for an explanation of each mode.  

`speed_table_index` The index of the camera speed table to use. This value is dependent on the camera. The speed table is logged as output when running the application, if you need to change this value refer to the output and select one of the speed table entries and restart.  

`[device.tango] settings`  
`com` The COM port for the connected stage.

`[acquisition] settings`  
These are the default acquisition settings, all these values can be changed from the application itself, changing the values in the application does not change the default values in the config file.  

`[acquisition.region] settings`  
This is the default region to capture, there is no interface in the UI to change these values.  

`s1` Serial register 1 value.  
`p1` Parallel register 1 value.  
`s2` Serial register 2 value.  
`p2` Parallel register 2 value.  
`sbin` Serial binning factor.  
`pbin` Parallel binning factor.  

Depending on the camera sensor the point (s1,p1) will either be the top left corner or the top right corner of the region. The point (s2, p2) will be the bottom right or bottom left respectfully. For more information about the coordinate model refer to the PVCAM documentation.  

## Running
When running the software you must add at least one stage location through the stage navigation modal, if there are no saved locations the software will not capture any images.  

Live view can be on during acquisition, if you start live view and then start an acquisition the live view will remain on after the acquisition is finished. You must stop live view in order to turn off the LED panel.  

Log files are stored under `C:\Users\<user>\AppData\Nautilus`
