[Redis Desktop Manager](http://redisdesktop.com "Redis Desktop Manager Offical Site")
===================
Ubuntu: [![Build Status](https://travis-ci.org/uglide/RedisDesktopManager.png?branch=master)](https://travis-ci.org/uglide/RedisDesktopManager) 
Mac OS X: [![Build Status](https://travis-ci.org/uglide/RedisDesktopManager.png?branch=osx)](https://travis-ci.org/uglide/RedisDesktopManager)

Open source cross-platform Redis Desktop Manager based on Qt 5


**Supported platforms**

- Windows Xp, Vista, 7, 8
- Mac OS X 10.8, 10.9
- Ubuntu 12 and 13
- Debian 7


## Install & Run ##

### Windows ###

1. Download Windows Installer from [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Run downloaded installer

### Mac OS X ###
1. Download dmg image from [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Mount dmg image
3. Run rdm.app

### Ubuntu & Debian ###

#### Using console ####

1. Download Debian Package from  [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Install package:
    `dpkg -i redis-desktop-manager_X.X.X_i386.deb`
3. Run RedisDesktopManager :
	`/usr/share/redis-desktop-manager/bin/rdm`

After installation Redis Desktop Manager will be available in main menu.

### Other Linux ###
You can build Redis Desktop Manager from source.

1. Install Qt 5.1.1
2. Install openssl 1.0
2. Run:
	`./configure && qmake redis-desktop-manager.pro && make`



