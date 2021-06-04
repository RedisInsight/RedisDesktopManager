# Quick Install

## Windows

1. Install [Microsoft Visual C++ 2015-2019 x64](https://aka.ms/vs/16/release/vc_redist.x64.exe)  (If you have not already).
2. Download Windows Installer from [http://rdm.dev/download](http://rdm.dev/download). **(Requires subscription)**
3. Run the downloaded installer.

## Mac OS X

1. Download dmg image from [http://rdm.dev/download](http://rdm.dev/download). **(Requires subscription)**
2. Mount the DMG image.
3. Run rdm.app.

## Ubuntu / ArchLinux / Debian / Fedora / CentOS / OpenSUSE / etc

1. Install RDM using [Snapcraft](https://snapcraft.io/redis-desktop-manager).

> !!! warning "SSH Keys"
    To be able to access your ssh keys from RDM please connect `ssh-key` interface:
    `sudo snap connect redis-desktop-manager:ssh-keys`
    
> !!! tip "How to Run"
    If RDM icon hasn't appeared in your application launcher you can run RDM from terminal `/snap/bin/redis-desktop-manager.rdm`

## Build from source

### Get source

1. Install git using the instructions here: https://git-scm.com/download
    
2. Get the source code:
    ```
    git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 2021 rdm && cd ./rdm
    ```

> !!! warning "SSH Tunneling support"
    Since 0.9.9 RDM by default does not include SSH Tunneling support. You can create a SSH tunnel to your Redis server manually and connect to `localhost`:
    `ssh -L 6379:REDIS_HOST:6379 SSH_USER@SSH_HOST -P SSH_PORT -i SSH_KEY -T -N` or [use pre-built binary for your OS](#quick-install)


### Build on OS X

1. Install [Xcode](https://developer.apple.com/xcode/) with Xcode build tools.
2. Install [Homebrew](http://brew.sh/).
3. Copy `cd ./src && cp ./resources/Info.plist.sample ./resources/Info.plist`.
4. Building RDM dependencies require i.a. `openssl`, `cmake` and `python3`. Install them: `brew install openssl cmake python3`
5. Build lz4 lib
```
cd 3rdparty/lz4/build/cmake
cmake -DLZ4_BUNDLED_MODE=ON  .
make
```
6. Install Python requirements `pip3 install -t ../bin/osx/release -r py/requirements.txt`
7. Install [Qt 5.15](http://www.qt.io/download-open-source/#section-2). Add Qt Creator and under Qt 5.15.x add Qt Charts module.
8. Open `./src/rdm.pro` in **Qt Creator**.
9. Run build. 

### Build on Windows

1. Install [Visual Studio 2019 Community Edition](https://visualstudio.microsoft.com/vs/).
2. Install [Qt 5.15](https://www.qt.io/download).
3. Go to `3rdparty/qredisclient/3rdparty/hiredis` and apply the patch to fix compilation on Windows:
`git apply ../hiredis-win.patch`
4. Go to the `3rdparty/` folder and install zlib with `nuget`: `nuget install zlib-msvc14-x64 -Version 1.2.11.7795`
5. Build lz4 lib
```
cd 3rdparty/lz4/build/cmake
cmake -DLZ4_BUNDLED_MODE=ON  .
make
```
6. Install Python 3.9 amd64 to `C:\Python39-x64`.
7. Install Python requirements `pip3 install -r src/py/requirements.txt`.
8. Open `./src/rdm.pro` in **Qt Creator**.  Choose the `Desktop Qt 5.15.x MSVC2019 64bit > Release` build profile.
9. Run build. (Just hit `Ctrl-B`)
