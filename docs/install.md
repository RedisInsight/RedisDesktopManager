# Quick Install

## Windows

#### Manual install (recommended)
1. Download Windows Installer from [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Run downloaded installer

#### Install via Chocolatey (outdated)
```bash
choco install redis-desktop-manager
```

## Mac OS X
#### Homebrew cask
```bash
# Install Homebrew cask
brew install caskroom/cask/brew-cask 

# Install Redis Desktop Manager aka RDM
brew cask install rdm 
```
#### Manual install
1. Download dmg image from [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Mount dmg image
3. Run rdm.app

## Ubuntu
1. Download Debian Package from  [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Install package:
    `dpkg -i redis-desktop-manager_X.X.X_i386.deb`
3. Run RedisDesktopManager :
	`/usr/share/redis-desktop-manager/bin/rdm` or `redis-desktop-manager`

After installation Redis Desktop Manager will be available in main menu.

Note: You may need the libicu52 package available from launchpad[https://launchpad.net/ubuntu/+source/icu]

## Fedora
[Build from source](install.md#build-on-linux) with automated bash script

## ArchLinux
Package - https://aur.archlinux.org/packages/redis-desktop-manager/

## Other platforms

You can [build Redis Desktop Manager from source](install.md#build-from-source).

## Build from source

### Get source
1. Install git
2. Get source code:
 
```bash
git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 0.8.0 rdm && cd ./rdm
```

### Build on Linux
#### Ubuntu
```bash
cd src/
./configure
source /opt/qt54/bin/qt55-env.sh && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
mv qt.conf qt.backup
```
#### Fedora
```bash
cd src/
./configure
qmake-qt5 && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
mv qt.conf qt.backup
```


### Build on OS X
1. Install XCode with xcode build tools
2. Build RDM dependencies `cd ./src && ./configure`
3. Install [Qt 5.5](http://www.qt.io/download-open-source/#section-2)
4. Open ./src/rdm.pro in Qt Creator
5. Run build

### Build on Windows##
1. Install Visual Studio 2013 Community with Update 5
2. Install [Qt 5.6+](http://www.qt.io/download-open-source/#section-2)
3. Download RDM dependencies `cd ./src` and  run `configure.bat`
4. Open ./src/rdm.pro in Qt Creator
5. Run build
