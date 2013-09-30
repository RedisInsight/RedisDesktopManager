#!/bin/sh
QTDIR=/usr/local/Qt-5.1.1
USER=$(whoami)
HOME_DIR=/home/$USER/


echo ===========================
echo Enter build dir :
echo ===========================
cd /vagrant
pwd

echo   
echo ===========================
TAG=$(git describe --tags $(git rev-list --tags --max-count=1))
echo Last tag: $TAG
echo ===========================

echo   
echo ===========================
echo copy source dir :

BUILD_DIR=$HOME_DIR/redis-desktop-manager-$TAG
rm -fR $BUILD_DIR
rm -f $HOME_DIR/redis-desktop*
mkdir $BUILD_DIR

cp -Rf /vagrant/redis-desktop-manager/* $BUILD_DIR
mkdir $BUILD_DIR/debian 
cp -Rf /vagrant/build/debian/* $BUILD_DIR/debian  
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
sudo cp -Rf $QTDIR/plugins/platforms/lib* $DEPS_LIB/plugins/platforms  
cp -Rf $QTDIR/lib/libQt5Xml.s* $DEPS_LIB
cp -Rf $QTDIR/lib/libQt5Widgets.s* $DEPS_LIB
cp -Rf $QTDIR/lib/libQt5Network.s* $DEPS_LIB
cp -Rf $QTDIR/lib/libQt5Gui.s* $DEPS_LIB
cp -Rf $QTDIR/lib/libQt5Core.s* $DEPS_LIB
cp -Rf $QTDIR/lib/libQt5DBus.s* $DEPS_LIB
#cp -Rf $QTDIR/lib/libqgsttools_p.s* $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libxcb* $DEPS_LIB
#cp -Rf /usr/lib/libicu* $DEPS_LIB


#external libs
cp -Rf /usr/local/lib/libssh2.s* $DEPS_LIB

echo 
echo ===========================
echo pack source:
echo ===========================
rm -fR $BUILD_DIR/Debug
rm -fR $BUILD_DIR/Release
rm -fR $BUILD_DIR/GeneratedFiles

cd $HOME_DIR

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

cd $HOME_DIR
cp *.deb /vagrant/







