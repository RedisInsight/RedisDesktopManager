#!/bin/sh

echo ============================
echo Set version $1
echo ============================
TAG=$1
python set_version.py $1 > ../src/version.h
python set_version.py $1 > ../3rdparty/crashreporter/src/version.h
echo DONE

echo ============================
echo Qt info
echo ============================
echo '# Used Qt:'
qmake -v
which qmake
cd ./../
SOURCE_DIR=`pwd`

echo ==========================
echo Clean
echo ==========================
rm -f redis-desktop-manager-*.gz
rm -f redis-desktop-manager-*.tar
rm -fR ./redis-desktop-manager-*/

echo
echo ===========================
echo Build Crash Reporter :
echo ===========================
cd ./3rdparty/crashreporter
qmake CONFIG+=release DESTDIR=$SOURCE_DIR/bin/linux/release
make clean
make

cd $SOURCE_DIR

echo
echo ===========================
echo Build Dependencies :
echo ===========================
cd ./src
chmod +x ./configure
./configure

cd ./../

echo   
echo ===========================
echo copy source dir :

BUILD_DIR=./redis-desktop-manager-$TAG
rm -fR $BUILD_DIR/*
mkdir $BUILD_DIR

cp -Rf ./src/* $BUILD_DIR
chmod +x $BUILD_DIR/configure
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
mkdir $DEPS_LIB/fonts

#external libs
sudo cp -Rf $QTDIR/lib/fonts/* $DEPS_LIB/fonts
cp -aR /usr/lib/i386-linux-gnu/libxcb*.s* $DEPS_LIB
cp -aR /usr/lib/`uname -m`-linux-gnu/libxcb*.s* $DEPS_LIB


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

echo 
echo ===========================
echo rename package
echo ===========================
cd $SOURCE_DIR
PLATFORM=$(uname -m)

BUILDERNAME=$(lsb_release -si)$(lsb_release -sr)

if [ "$PLATFORM" != 'x86_64' ]; then
    PLATFORM='i386'
else 
    PLATFORM='amd64'
fi

PACKAGESUFIX=$TAG"_"$PLATFORM

FILENAME="redis-desktop-manager_"$PACKAGESUFIX".deb"
NEW_FILE="redis-desktop-manager_"$BUILDERNAME"_"$PACKAGESUFIX".deb"

echo "# New Package Name : $NEW_FILE"

cp $FILENAME $NEW_FILE

rm -f $FILENAME

