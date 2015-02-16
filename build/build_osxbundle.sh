#!/bin/sh

echo ============================
echo Set version $1
echo ============================
TAG=$1
python ./build/set_version.py $1 > ./src/version.h
python ./build/set_version.py $1 > ./3rdparty/crashreporter/src/version.h
echo DONE

echo ============================
echo Setup Build Environment
echo ============================
QTPATH=/usr/local/opt/qt5/bin/
export PATH=$PATH:$QTPATH
SOURCE_DIR=`pwd`
rm -fR ./bin/linux/release/*
echo DONE


echo ============================
echo Build project
echo ============================
echo Build Crash Reporter :
cd ./3rdparty/crashreporter
qmake -v
qmake CONFIG+=release DESTDIR=$SOURCE_DIR/bin/linux/release
make -s clean
make -s -j 2
cd $SOURCE_DIR

echo ===========================
echo Build Application :
cd ./src

#replace tag in Info.plist:
cp resources/Info.plist.sample resources/Info.plist
sed -i "s/0.0.0/$TAG/g" resources/Info.plist

sh ./configure
qmake CONFIG-=debug CONFIG+=release CONFIG+=app_bundle
make -s clean
make -s -j 2

echo DONE

echo ============================
echo Create release bundle
echo ============================
cd $SOURCE_DIR

BUNDLE_PATH=./bin/linux/release/ 
BUILD_DIR=$BUNDLE_PATH/rdm.app/Contents/
MAC_TOOL=$QTPATH/macdeployqt

cp -f ./src/resources/Info.plist $BUILD_DIR/
cp -f ./src/resources/rdm.icns $BUILD_DIR/Resources/

cd $BUNDLE_PATH

$MAC_TOOL rdm.app -dmg -executable=./rdm.app/Contents/MacOS/crashreporter
cp rdm.dmg redis-desktop-manager-$TAG.dmg

echo ============================
echo Export symbols
echo ============================
cd $SOURCE_DIR/3rdparty/breakpad/
xcodebuild -sdk macosx10.9 -project src/tools/mac/dump_syms/dump_syms.xcodeproj -configuration Release ARCHS=x86_64 ONLY_ACTIVE_ARCH=YES MACOSX_DEPLOYMENT_TARGET=10.9 GCC_VERSION=com.apple.compilers.llvm.clang.1_0
src/tools/mac/dump_syms/build/Release/dump_syms $SOURCE_DIR/bin/linux/release/rdm.app/Contents/MacOS/rdm > $SOURCE_DIR/bin/linux/release/rdm.sym
