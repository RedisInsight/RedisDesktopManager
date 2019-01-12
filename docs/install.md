# Quick Install

## Windows

0. Install [Microsoft Visual C++ 2015](https://download.microsoft.com/download/9/3/F/93FCF1E7-E6A4-478B-96E7-D4B285925B00/vc_redist.x86.exe)  (If you have not already)
1. Download Windows Installer from [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
2. Run downloaded installer

## Mac OS X

1. Download dmg image from [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
2. Mount dmg image
3. Run rdm.app

## Ubuntu / Debian / Fedora / CentOS / OpenSUSE / Other Linux

1. Install RedisDesktopManager using [Snapcraft](https://snapcraft.io/redis-desktop-manager)

## ArchLinux

1. Install RedisDesktopManager via [AUR](https://aur.archlinux.org/packages/redis-desktop-manager/)

## Other platforms

You can [build Redis Desktop Manager from source](install.md#build-from-source).

## Build from source

### Get source

1. Install git
2. Get source code:

    ```
    git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 0.9 rdm && cd ./rdm
    ```

> !!! warning "SSH Tunneling support"
    Since 0.9.9 RDM by default do not include SSH Tunneling support. If you need it please checkout 0.9.8 tag.
    `git checkout 0.9.8`

### Build on Linux

#### Ubuntu

```
cd src/
./configure
qmake && make && sudo make install
cd /opt/redis-desktop-manager/
sudo mv qt.conf qt.backup
```

#### Fedora & CentOS & OpenSUSE

```
cd src/
./configure
qmake-qt5 && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
sudo mv qt.conf qt.backup
```

> !!! tip "Tip: checkinstall"
    Instead of `sudo make install` consider to use `sudo checkinstall -D --install` on **deb**-based OS and `sudo checkinstall -R --install` on **rpm**-based OS to generate package and install it into the system.

### Build on OS X

1. Install [XCode](https://developer.apple.com/xcode/) with Xcode build tools
2. Install [Homebrew](http://brew.sh/)
3. Copy `cd ./src && cp ./resources/Info.plist.sample ./resources/Info.plist`
4. Building RDM dependencies require i.a. openssl and cmake. Install them: `brew install openssl cmake`
5. Build RDM dependencies `./configure`
6. Install [Qt 5.9](http://www.qt.io/download-open-source/#section-2). Add Qt Creator and under Qt 5.9.x add Qt Charts module.
7. Open ./src/rdm.pro in Qt Creator
8. Run build

### Build on Windows

1. Install Visual Studio 2015 Community with Updates

2. Install [Qt 5.9](https://www.qt.io/download)

3. Go to `3rdparty/qredisclient/3rdparty/hiredis` and apply patch to fix compilation on Windows:
`git apply ../hiredis-win.patch`

4. Open `./src/rdm.pro` in **Qt Creator**.  Chooses `Desktop Qt 5.9.6 MSVC2015 32bit > Release` profile.

5. Run build. ( Just hit `Ctrl-B` )
