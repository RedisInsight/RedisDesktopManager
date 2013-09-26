#!/bin/sh

echo ===========================
echo Enter build dir :
echo ===========================
cd /vagrant/build
pwd

echo   
echo ===========================
TAG=$(git describe --tags $(git rev-list --tags --max-count=1))
echo Last tag: $TAG
echo ===========================


echo   
echo ===========================
echo copy source dir :
mkdir ./redis-desktop-manager-$TAG
cp -Rf /vagrant/redis-desktop-manager/* ./redis-desktop-manager-$TAG
mkdir ./redis-desktop-manager-$TAG/debian 
cp -Rf /vagrant/build/debian/* ./redis-desktop-manager-$TAG/debian  
echo ===========================

echo   
echo ===========================
echo replace tag in debian/changelog:
sed -i -e "s,0.0.0,$TAG,g" ./redis-desktop-manager-$TAG/debian/changelog
echo ===========================

echo 
echo ===========================
echo copy libs:
echo ===========================
USER=$(whoami)
DEPS_LIB=/home/$USER/rdmlib/

mkdir $DEPS_LIB

#qt libs
mkdir $DEPS_LIB/plugins
sudo cp -Rf /usr/lib/i386-linux-gnu/qt5/plugins/platforms $DEPS_LIBplugins/platforms  
cp -Rf /usr/lib/i386-linux-gnu/libQt5Xml.s* $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libQt5Widgets.so $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libQt5Network.s* $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libQt5Gui.s* $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libQt5Core.s* $DEPS_LIB
cp -Rf /usr/lib/i386-linux-gnu/libqgsttools_p.s* $DEPS_LIB
cp -Rf /usr/lib/libicu* $DEPS_LIB

#external libs
cp -Rf /usr/local/lib/libssh2.s* $DEPS_LIB

#change libs path in project file
sed -i -e 's,data.files = lib/*,data.files = /home/vargant/rdmlib/,g' ./redis-desktop-manager-$TAG/redis-desktop-manager.pro

echo 
echo ===========================
echo pack source:
echo ===========================
rm -fR ./redis-desktop-manager-$TAG/Debug
rm -fR ./redis-desktop-manager-$TAG/Release
rm -fR ./redis-desktop-manager-$TAG/GeneratedFiles

tar czvf redis-desktop-manager-$TAG.tar.gz redis-desktop-manager-$TAG
cp redis-desktop-manager-$TAG.tar.gz redis-desktop-manager_$TAG.orig.tar.gz
echo ===========================

echo 
echo ===========================
echo build package
echo ===========================
cd ./redis-desktop-manager-$TAG

DEBEMAIL="u.glide@gmail.com"
DEBFULLNAME="Igor Malinovskiy"
export DEBEMAIL DEBFULLNAME

debuild -uc -us







