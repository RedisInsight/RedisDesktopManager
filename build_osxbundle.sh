#!/bin/sh

BUNDLE_PATH=./bin/linux/release/ 
BUILD_DIR=$BUNDLE_PATH/rdm.app/Contents/
MAC_TOOL=/Users/admin/Qt5.1.1/5.1.1/clang_64/bin/macdeployqt

cp -f ./redis-desktop-manager/Info.plist $BUILD_DIR/
cp -f ./redis-desktop-manager/rdm.icns $BUILD_DIR/Resources/

cd $BUNDLE_PATH

$MAC_TOOL rdm.app -dmg 
