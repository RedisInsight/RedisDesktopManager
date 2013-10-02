#!/bin/sh

#install gcc 4.7

sudo sh -c "echo 'deb http://ftp.ua.debian.org/debian wheezy main' >> /etc/apt/sources.list"

sudo sh -c "echo 'Package: *' > /etc/apt/preferences"
sudo sh -c "echo 'Pin: release n=squeeze' >> /etc/apt/preferences"
sudo sh -c "echo 'Pin-Priority: 900' >> /etc/apt/preferences"
sudo sh -c "echo '' >> /etc/apt/preferences"
sudo sh -c "echo 'Package: *' >> /etc/apt/preferences"
sudo sh -c "echo 'Pin: release n=wheezy' >> /etc/apt/preferences"
sudo sh -c "echo 'Pin-Priority: 200' >> /etc/apt/preferences"

sudo apt-get update
sudo apt-get -y --target-release testing install gcc
