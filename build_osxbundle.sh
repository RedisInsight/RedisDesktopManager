#!/bin/sh
QTPATH=/Users/admin/Qt/5.3/clang_64/bin

export PATH=$PATH:$QTPATH

echo ===========================
TAG=$1
echo Version: $TAG
echo ===========================

rm -fR ./bin/linux/release/*


echo ‘============== Build crash reporter ================’
cd ./crashreporter
qmake “CONFIG+=release CONFIG-=debug”
make -s clean
make -s -j 2

echo ‘============== Build rdm ================’
cd ./../redis-desktop-manager/
pwd
sh ./configure
qmake
make -s clean
make -s -j 2

echo ‘============== Create release bundle ================’
cd ./../

BUNDLE_PATH=./bin/linux/release/ 
BUILD_DIR=$BUNDLE_PATH/rdm.app/Contents/
MAC_TOOL=$QTPATH/macdeployqt

cp -f ./redis-desktop-manager/resources/Info.plist $BUILD_DIR/
cp -f ./redis-desktop-manager/resources/rdm.icns $BUILD_DIR/Resources/

cd $BUNDLE_PATH

$MAC_TOOL rdm.app -dmg 
cp rdm.dmg redis-desktop-manager-$TAG.dmg
