#!/bin/sh

echo ============================
echo Set version $APP_VERSION
echo ============================
TAG=$APP_VERSION
python set_version.py $APP_VERSION > ../src/version.h
python set_version.py $APP_VERSION > ../3rdparty/crashreporter/src/version.h
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
qmake CONFIG+=release DESTDIR=$SOURCE_DIR/bin/linux/release QMAKE_LFLAGS_RPATH=""
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
chmod +x $BUILD_DIR/resources/rdm.sh
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
PLUGINS=$DEPS_LIB/plugins/

mkdir $DEPS_LIB
mkdir -p $PLUGINS

RDM_QT_LIB_PATH=/opt/qt55/

#external libs
cp -L $RDM_QT_LIB_PATH/lib/libQt5QuickWidgets.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Widgets.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Gui.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Qml.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Network.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Core.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Quick.so.5  $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5DBus.so.5 $DEPS_LIB
cp -L $RDM_QT_LIB_PATH/lib/libQt5Concurrent.so.5 $DEPS_LIB
cp -LR $RDM_QT_LIB_PATH/plugins/platforms/ $PLUGINS
cp -LR $RDM_QT_LIB_PATH/plugins/imageformats/ $PLUGINS
cp -R $RDM_QT_LIB_PATH/qml $PLUGINS

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

echo ===========================
echo export symbols
echo ===========================
cd $SOURCE_DIR/3rdparty/breakpad
cp -r src/src .
./configure
make
cd src/tools/linux/dump_syms
./dump_syms $SOURCE_DIR/bin/linux/release/rdm > $SOURCE_DIR/bin/linux/release/rdm.sym
