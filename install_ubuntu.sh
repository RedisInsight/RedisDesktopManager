#!/bin/sh

sudo apt-get update 
sudo apt-get install git python perl libssl0.9.8 -y
sudo apt-get install "^libxcb.*" libx11-xcb-dev libglu1-mesa-dev libxrender-dev libatspi libdbus-1-dev -y 

#setup build env
sudo apt-get install build-essential devscripts ubuntu-dev-tools debhelper \
        dh-make diff patch gnupg fakeroot lintian pbuilder -y

cd /home/vagrant

echo 'COMPONENTS="main restricted universe multiverse"' > ~/.pbuilderrc

sudo pbuilder create

#get and build qt

QTBUILD_DIR=/home/vagrant/qt
INSTALL_ARC=$QTBUILD_DIR/qt-everywhere-opensource-src-5.1.1.tar.gz

echo $INSTALL_ARC

if [ ! -f $INSTALL_ARC ]
then
	wget http://download.qt-project.org/official_releases/qt/5.1/5.1.1/single/qt-everywhere-opensource-src-5.1.1.tar.gz	
	tar -xvf $INSTALL_ARC
fi

cd $QTBUILD_DIR/qt-everywhere-opensource-src-5.1.1

sudo ./configure -opensource -qt-libpng -qt-libjpeg -qt-xcb -qt-xkbcommon -no-kms -no-opengl -dbus-linked -nomake examples -confirm-license

sudo make
sudo make install
