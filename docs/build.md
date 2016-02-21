## Get source
1. Install git
2. Get source code:
 
```
git clone --recursive https://github.com/uglide/RedisDesktopManager.git -b 0.8.0 rdm && cd ./rdm
```

## Build on Linux
### Ubuntu
```
cd src/
./configure
source /opt/qt54/bin/qt55-env.sh && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
mv qt.conf qt.backup
```
### Fedora
```
cd src/
./configure
qmake-qt5 && make && sudo make install
cd /usr/share/redis-desktop-manager/bin
mv qt.conf qt.backup
```


## Build on OS X
1. Install XCode with xcode build tools
2. Build RDM dependencies `cd ./src && ./configure`
3. Install [Qt 5.5](http://www.qt.io/download-open-source/#section-2)
4. Open ./src/rdm.pro in Qt Creator
5. Run build

## Build on Windows [work in progress] ##
1. Install Visual Studio 2013 Community with Update 5
2. Download Nuget command line tool
...
2. Install [Qt 5.6+](http://www.qt.io/download-open-source/#section-2)
3. Open ./src/rdm.pro in Qt Creator
4. Run build