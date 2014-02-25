#!/bin/sh
QTDIR=/usr/local/Qt-5.1.1

export PATH=$PATH:$QTDIR/bin

echo ===========================
echo build dir :
echo ===========================
SOURCE_DIR=`pwd`
pwd

echo ===========================
echo Build Crash Reporter :
echo ===========================
cd ./crashreporter
qmake
make clean
make

cd ./../

cp ./bin/linux/release/crashreporter ./bin/linux/release/

echo   
echo ===========================
TAG=$1
echo Last tag: $TAG
echo ===========================

echo   
echo ===========================
echo copy source dir :

BUILD_DIR=./redis-desktop-manager-$TAG
rm -fR $BUILD_DIR/*
mkdir $BUILD_DIR

cp -Rf ./redis-desktop-manager/* $BUILD_DIR
chmod 755 $BUILD_DIR/configure
mkdir $BUILD_DIR/debian 
cp -Rf ./build/debian/* $BUILD_DIR/debian  
echo ===========================

echo   
echo ===========================
echo replace tag in debian/changelog:
sed -i -e "s,0.0.0,$TAG,g" $BUILD_DIR/debian/changelog
echo ===========================

echo 
echo ===========================
echo copy libs:
echo ===========================
DEPS_LIB=$BUILD_DIR/lib

mkdir $DEPS_LIB

#qt libs
mkdir $DEPS_LIB/plugins
mkdir $DEPS_LIB/plugins/platforms
mkdir $DEPS_LIB/fonts
sudo cp -Rf $QTDIR/plugins/platforms/lib* $DEPS_LIB/plugins/platforms  
sudo cp -Rf $QTDIR/lib/fonts/* $DEPS_LIB/fonts  
cp -aR $QTDIR/lib/libQt5Xml.so.5.1.1 $DEPS_LIB/libQt5Xml.so.5
cp -aR $QTDIR/lib/libQt5Widgets.so.5.1.1 $DEPS_LIB/libQt5Widgets.so.5
cp -aR $QTDIR/lib/libQt5Network.so.5.1.1 $DEPS_LIB/libQt5Network.so.5
cp -aR $QTDIR/lib/libQt5Gui.so.5.1.1 $DEPS_LIB/libQt5Gui.so.5
cp -aR $QTDIR/lib/libQt5Core.so.5.1.1 $DEPS_LIB/libQt5Core.so.5
cp -aR $QTDIR/lib/libQt5DBus.s* $DEPS_LIB
cp -aR /usr/lib/`uname -m`-linux-gnu/libxcb*.s* $DEPS_LIB

#external libs
cp -aR /usr/local/lib/libssh2.s* $DEPS_LIB


echo 
echo ===========================
echo pack source:
echo ===========================
rm -fR $BUILD_DIR/Debug
rm -fR $BUILD_DIR/Release
rm -fR $BUILD_DIR/GeneratedFiles

cd $SOURCE_DIR

tar czvf redis-desktop-manager-$TAG.tar.gz $BUILD_DIR
cp redis-desktop-manager-$TAG.tar.gz redis-desktop-manager_$TAG.orig.tar.gz
echo ===========================


echo 
echo ===========================
echo build package
echo ===========================
cd $BUILD_DIR

DEBEMAIL="u.glide@gmail.com"
DEBFULLNAME="Igor Malinovskiy"
export DEBEMAIL DEBFULLNAME

#debuild -uc -us
dpkg-buildpackage -b








