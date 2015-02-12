#!/bin/bash
username=test
password=test
pass=$(perl -e 'print crypt($ARGV[0], "password")' $password)
echo "Password: $pass"
sudo useradd -m -p $pass $username
echo test:test | chpasswd

sudo cp /etc/ssh/sshd_config /root/sshd_config
sudo bash -c "echo '' >> /root/sshd_config"
sudo bash -c "echo 'AllowUsers test' >> /root/sshd_config"
sudo sed -i "s/Port 22/Port 2222/g" /root/sshd_config
sudo sed -i "s/\/etc\/ssh\//~\//g" /root/sshd_config

sudo ssh-keygen -t rsa -N "" -f /root/ssh_host_rsa_key
sudo ssh-keygen -t dsa -N "" -f /root/ssh_host_dsa_key
sudo ssh-keygen -t ecdsa -N "" -f /root/ssh_host_ecdsa_key

sudo `which sshd` -f /root/sshd_config
ps aux | grep ssh
