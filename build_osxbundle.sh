#!/bin/sh

BUNDLE_PATH=./bin/linux/release/rdm.app 
BUILD_DIR=$BUNDLE_PATH/Contents/
MAC_TOOL=/Users/administrator/Qt5.1.1/5.1.1/clang_64/bin/macdeployqt

cp -f ./redis-desktop-manager/Info.plist $BUILD_DIR/
cp -f ./redis-desktop-manager/rdm.icns $BUILD_DIR/Resources/

$MAC_TOOL $BUNDLE_PATH -dmg 
