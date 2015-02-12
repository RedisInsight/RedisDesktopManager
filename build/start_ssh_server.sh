#!/bin/bash
username=test
password=test
pass=$(perl -e 'print crypt($ARGV[0], "password")' $password)
echo "Password: $pass"
sudo useradd -m -p $pass $username
echo test:test | chpasswd

cp /etc/ssh/sshd_config ~/sshd_config
bash -c "echo '' >> ~/sshd_config"
bash -c "echo 'AllowUsers test' >> ~/sshd_config"
sed -i "s/Port 22/Port 2222/g" ~/sshd_config
sed -i "s/\/etc\/ssh\//~\//g" ~/sshd_config

ssh-keygen -t rsa -N "" -f ~/ssh_host_rsa_key
ssh-keygen -t dsa -N "" -f ~/ssh_host_dsa_key
ssh-keygen -t ecdsa -N "" -f ~/ssh_host_ecdsa_key

`which sshd` -f ~/sshd_config
ps aux | grep ssh
