#!/bin/sh
QT_VERSION=5.3.0
QT_INSTALLER_NAME=qt-everywhere-opensource-src-$QT_VERSION
QTDIR=/usr/local/Qt-$QT_VERSION
QT_DOWNLOAD=http://download.qt-project.org/official_releases/qt/5.3/5.3.0/single/qt-everywhere-opensource-src-5.3.0.tar.gz

sudo apt-get update 
sudo apt-get upgrade
sudo apt-get install git python perl -y
sudo apt-get install libssl-dev -y
sudo apt-get install "^libxcb.*" libx11-xcb-dev libglu1-mesa-dev libxrender-dev -y 
sudo apt-get install libatspi-dev -y
sudo apt-get install libxext-dev -y
sudo apt-get install g++ -y
sudo apt-get install automake -y
sudo apt-get install build-essential devscripts ubuntu-dev-tools debhelper \
        dh-make diffutils patch gnupg fakeroot lintian pbuilder -y

echo '====================================='
echo "Qt Dir: $QTDIR"

if [ ! -d "$QTDIR" ]; then #get and build qt

  echo 'Qt not found. Install'
  
  QTBUILD_DIR=`pwd`/qt
  mkdir $QTBUILD_DIR

  INSTALL_ARC=$QTBUILD_DIR/$QT_INSTALLER_NAME.tar.gz
  echo $INSTALL_ARC
  cd $QTBUILD_DIR

  if [ ! -f $INSTALL_ARC ]
  then
    wget $QT_DOWNLOAD		
  fi

  rm -fR ./$QT_INSTALLER_NAME
  tar -xvf $INSTALL_ARC
  cd ./$QT_INSTALLER_NAME

  sudo ./configure -opensource -release -static  -qt-libpng -qt-libjpeg -qt-xcb -qt-xkbcommon -no-kms -no-opengl -openssl-linked -nomake examples -nomake tests -v -skip webkit -skip multimedia -skip declarative -confirm-license

  sudo make
  sudo make install

  export PATH=$PATH:/usr/local/$QT_VERSION/bin

  #setup build env
  echo 'COMPONENTS="main restricted universe multiverse"' > ~/.pbuilderrc
  sudo pbuilder create
fi
