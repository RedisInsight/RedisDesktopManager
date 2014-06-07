#!/bin/sh

wget http://ftp.gnu.org/gnu/automake/automake-1.14.tar.gz
tar -zxvf automake-1.14.tar.gz
chmod -r 755 automake-1.14
cd automake-1.14
./configure
make
sudo make install