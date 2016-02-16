#### Why I get error : _db X (Loaded XXX of YYY. Error - XXXXXX)_
You get this error if connection error occurred, or if you use unofficial port of redis for Windows - stable work of Redis Desktop Manager guaranteed only for latest version of Official Microsoft Redis port  ([https://github.com/MSOpenTech/redis/tree/2.6/bin/release](https://github.com/MSOpenTech/redis/tree/2.6/bin/release))
 
#### Black screen instead of Welcome tab on Windows under VirtualBox
Remove opengl32sw.dll and Copy [this dll](https://github.com/uglide/RedisDesktopManager/blob/fix_improve_unit_tests/build/windows/installer/resources/rdp_fix/opengl32.dll?raw=true9) to RDM installation folder

#### Graphical glitches on Ubuntu (RDM 0.7.9+)
It's the well-known problem of Qt Quick components.