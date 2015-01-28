##[Redis Desktop Manager](http://redisdesktop.com "Redis Desktop Manager Offical Site") [![Build Status](https://travis-ci.org/uglide/RedisDesktopManager.png?branch=0.8.0)](https://travis-ci.org/uglide/RedisDesktopManager) [![Coverage Status](https://coveralls.io/repos/uglide/RedisDesktopManager/badge.png?branch=0.8.0)](https://coveralls.io/r/uglide/RedisDesktopManager?branch=0.8.0) [![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/uglide/RedisDesktopManager?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
Open source cross-platform Redis Desktop Manager based on Qt 5

![](http://redisdesktop.com/img/features/all.png)

**Supported platforms**

- Windows Vista, 7, 8, 8.1
- Mac OS X 10.9+
- Ubuntu 12 LTS and 14 LTS
- Debian 7.5

**Supported Redis versions** : 2.4, 2.6, 2.8+

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
    `dpkg -i redis-desktop-manager_X.X.X_i386.deb && apt-get -f install`
4. Run RedisDesktopManager :
	`/usr/share/redis-desktop-manager/bin/rdm`

After installation Redis Desktop Manager will be available in main menu.

### Other Linux ###
You can build Redis Desktop Manager from source.

1. Install Qt 5.4+
2. Install openssl 1.0.0+
3. Install libssh2
4. Run:
	`qmake ./src/rdm.pro && make`



