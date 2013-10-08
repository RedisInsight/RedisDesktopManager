#!/bin/sh

sudo apt-get update 
sudo apt-get install python-software-properties -y
sudo apt-add-repository ppa:ubuntu-sdk-team/ppa -y
sudo apt-get update
sudo apt-get install ubuntu-sdk -y
sudo apt-get install libssl1.0.0 -y
sudo apt-get install git -y
