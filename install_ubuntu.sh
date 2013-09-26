#!/bin/sh

sudo apt-get update 
sudo apt-get install python-software-properties -y
sudo apt-add-repository ppa:ubuntu-sdk-team/ppa -y
sudo apt-get update
sudo apt-get install ubuntu-sdk -y
sudo apt-get install libssl0.9.8 -y
sudo apt-get install git -y

#setup build env
sudo apt-get install build-essential devscripts ubuntu-dev-tools debhelper \
        dh-make diff patch gnupg fakeroot lintian pbuilder -y

cd /home/vagrant

echo 'COMPONENTS="main restricted universe multiverse"' > ~/.pbuilderrc

sudo pbuilder create
