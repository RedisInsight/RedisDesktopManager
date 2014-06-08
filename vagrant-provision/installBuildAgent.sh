#!/bin/sh

cd ~/

echo 'Install CI Build Agent'
CI_SERVER=$1
echo "CI: server = $CI_SERVER"


echo '# Download build agent files'
wget -v http://$CI_SERVER/update/buildAgent.zip
unzip -f buildAgent.zip -d buildAgent
rm -f buildAgent.zip

echo '# Install JDK'
sudo apt-get install default-jdk -y

echo '# Update build agent options & run'
cd ./buildAgent/conf
cp buildAgent.dist.properties buildAgent.properties
sed -i -e "s,localhost:8111,$CI_SERVER,g" buildAgent.properties
cd ./../bin
chmod u+x *.sh
echo "`pwd`/agent.sh start" > /etc/rc.local
./agent.sh start
