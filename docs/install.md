# Quick Install

## Windows
0. Install [Microsoft Visual C++ 2015](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe)  (If you have not already)
1. Download Windows Installer from [http://redisdesktop.com/download](http://redisdesktop.com/download)
2. Run downloaded installer


## Mac OS X
1. Download dmg image from [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
2. Mount dmg image
3. Run rdm.app


## Ubuntu
1. Download `deb` package from  [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
2. Install package via Ubuntu Software Center
3. Run RedisDesktopManager :
	`/usr/share/redis-desktop-manager/bin/rdm` or `redis-desktop-manager`

After installation Redis Desktop Manager will be available in main menu.


## Fedora
[Build from source](install.md#build-on-linux) with automated bash script

## CentOS
[Build from source](install.md#build-on-linux) with automated bash script

## OpenSUSE
[Build from source](install.md#build-on-linux) with automated bash script

## ArchLinux
Package - [https://aur.archlinux.org/packages/redis-desktop-manager/](https://aur.archlinux.org/packages/redis-desktop-manager/)

## Other platforms

You can [build Redis Desktop Manager from source](install.md#build-from-source).

## Build from source

### Get source
1. Install git
2. Get source code:
 
```bash
git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 0.9 rdm && cd ./rdm
```

### Build on Linux
#### Ubuntu
```bash
cd src/
./configure
qmake && make && sudo make install
cd /opt/redis-desktop-manager/
sudo mv qt.conf qt.backup
```
#### Fedora & CentOS & OpenSUSE
```bash
cd src/
./configure
qmake-qt5 && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
sudo mv qt.conf qt.backup
```
!!! tip "Tip: checkinstall"
    Instead of `sudo make install` consider to use `sudo checkinstall -D --install` on **deb**-based OS and `sudo checkinstall -R --install` on **rpm**-based OS to generate package and install it into the system.

### Build on OS X
1. Install [XCode](https://developer.apple.com/xcode/) with Xcode build tools
2. Install [Homebrew](http://brew.sh/)
3. Copy `cd ./src && cp ./resources/Info.plist.sample ./resources/Info.plist`
4. Building RDM dependencies require i.a. openssl and cmake. Install them: `brew install openssl cmake`
4. Build RDM dependencies `./configure`
5. Install [Qt 5.9](http://www.qt.io/download-open-source/#section-2). Add Qt Creator and under Qt 5.9.x add Qt Charts module.
6. Open ./src/rdm.pro in Qt Creator
7. Run build

### Build on Windows##
1. Install Visual Studio 2015 Community with Update 2
2. Install [Qt 5.9](http://www.qt.io/download-open-source/#section-2)
3. Download RDM dependencies `cd ./src` and  run `configure.bat`
4. Open ./src/rdm.pro in Qt Creator
5. Run build
