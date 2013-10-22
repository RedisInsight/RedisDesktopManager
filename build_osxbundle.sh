#!/bin/sh

BUILD_DIR=./bin/linux/release/rdm.app/Contents/

cp -f ./redis-desktop-manager/Info.plist $BUILD_DIR/Info.plist
cp -f ./redis-desktop-manager/rdm.icns $BUILD_DIR/Resources/rdm.icns 
