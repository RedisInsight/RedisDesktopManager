#!/bin/sh

echo 'Install CI Build Agent'
CI_SERVER=$1
echo "CI: server = $CI_SERVER"


if [[ $OSTYPE == darwin* ]]; then
  brew install wget
fi

echo '# Download build agent files'
mkdir buildAgent
cd buildAgent
wget -v http://$CI_SERVER/update/buildAgent.zip
unzip buildAgent.zip
rm -f buildAgent.zip


echo '# Install JDK'

if [[ $OSTYPE == darwin* ]]; then
  brew install Caskroom/cask/java
else
  sudo apt-get install default-jdk -y
fi

echo '# Update build agent options & run'
cd ./conf
cp buildAgent.dist.properties buildAgent.properties
sed -i -e "s,localhost:8111,$CI_SERVER,g" buildAgent.properties
cd ./../bin
chmod u+x *.sh
echo "`pwd`/agent.sh start" > /etc/rc.local
./agent.sh start
