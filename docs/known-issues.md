### Black screen instead of Welcome tab on Windows under VirtualBox
Remove opengl32sw.dll and Copy [this dll](https://github.com/uglide/RedisDesktopManager/blob/5f99aa22aa84dc55b5c9858de28d68facd1c58b7/build/windows/installer/resources/rdp_fix/opengl32.dll) to RDM installation folder

### Black screen instead of Welcome tab on Windows Remote Desktop
You need to enable true color in RDP settings.

In Remote Desktop Connection Manager, set Server Properties/Display Settings/Default Settings Groupt Properties/Remote Desktop Settings/Color Depth to 32

### Black screen instead of Welcome tab on Linux
RDM starting from 0.8.0+ requires working openGL driver. Run `redis-desktop-manager` from terminal and search for error:
`libGL error: failed to load driver: XXX`

Also you can easily verify OpenGL support by e.g. running the `glxgears` demo application.

**Solution:** Reinstall your graphics driver

### Application cannot start (0xc000007b). Click OK to close the application
Download [`vcredist_x86.exe`](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe) and click `Repair`.

### Graphical glitches on Ubuntu (RDM 0.8.0+)
It's the well-known problem of Qt Quick components.

### RDM crashed on startup (Windows & RDM 0.8.6+)
RDM requires OpenGL 2.1 or higher to run. 

What you can do:
* On Windows PC/Laptop: Upgrade your graphics drivers
* On Windows Virtual machine: Enable 3d acceleration 
