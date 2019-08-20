# Quick Install

## Windows

1. Install [Microsoft Visual C++ 2017 x64](https://aka.ms/vs/15/release/vc_redist.x64.exe)  (If you have not already)
2. Download Windows Installer from [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
3. Run downloaded installer

## Mac OS X

1. Download dmg image from [http://redisdesktop.com/download](http://redisdesktop.com/download) **(Requires subscription)**
2. Mount dmg image
3. Run rdm.app

## Ubuntu / ArchLinux / Debian / Fedora / CentOS / OpenSUSE / etc

1. Install RedisDesktopManager using [Snapcraft](https://snapcraft.io/redis-desktop-manager)

> !!! warning "SSH Keys"
    To be able to access your ssh keys from RDM please connect `ssh-key` interface:
    `sudo snap connect redis-desktop-manager:ssh-keys`

## Build from source

### Get source

1. Install git
2. Get source code:

    ```
    git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 2019 rdm && cd ./rdm
    ```

> !!! warning "SSH Tunneling support"
    Since 0.9.9 RDM by default does not include SSH Tunneling support. You can create a SSH tunnel to your Redis server manually and connect to `localhost`:
    `ssh -L 6379:REDIS_HOST:6379 SSH_USER@SSH_HOST -P SSH_PORT -i SSH_KEY -T -N` or [use pre-built binary for your OS](#quick-install)


### Build on OS X

1. Install [XCode](https://developer.apple.com/xcode/) with Xcode build tools
2. Install [Homebrew](http://brew.sh/)
3. Copy `cd ./src && cp ./resources/Info.plist.sample ./resources/Info.plist`
4. Building RDM dependencies require i.a. `openssl`, `cmake` and `python3`. Install them: `brew install openssl cmake python3`
5. Install python requirements `pip3 install -t ../bin/osx/release -r py/requirements.txt`
6. Install [Qt 5.9](http://www.qt.io/download-open-source/#section-2). Add Qt Creator and under Qt 5.9.x add Qt Charts module.
7. Open ./src/rdm.pro in Qt Creator
8. Run build

### Build on Windows

1. Install Visual Studio 2017 Community

2. Install [Qt 5.9](https://www.qt.io/download)

3. Go to `3rdparty/qredisclient/3rdparty/hiredis` and apply patch to fix compilation on Windows:
`git apply ../hiredis-win.patch`

4. Go to `3rdparty/` folder and install zlib with `nuget`: `nuget install zlib-msvc14-x64 -Version 1.2.11.7795`

5. Install Python 3.7 amd64 to `C:\Python37-x64`

6. Install python requirements `pip3 install -r src/py/requirements.txt`

7. Open `./src/rdm.pro` in **Qt Creator**.  Chooses `Desktop Qt 5.9.x MSVC2017 64bit > Release` profile.

8. Run build. ( Just hit `Ctrl-B` )
