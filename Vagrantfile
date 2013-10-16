# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|

  #ubuntu 12 debug
  config.vm.define "ubu12debug" , primary: true do |ubu12debug|
    ubu12debug.vm.box = "precise32"
    ubu12debug.vm.box_url = "http://files.vagrantup.com/precise32.box"
    ubu12debug.vm.provision :shell, :path => "install_ubuntu-debug.sh"
  end 

  #ubuntu 12
  config.vm.define "ubu12" do |ubu12|
    ubu12.vm.box = "precise32"
    ubu12.vm.box_url = "http://files.vagrantup.com/precise32.box"
    ubu12.vm.provision :shell, :path => "install_ubuntu.sh"
  end 

  #debian 6
  config.vm.define "debi6" do |debi6|
    debi6.vm.box = "debi6"
    debi6.vm.box_url = "https://s3-eu-west-1.amazonaws.com/ffuenf-vagrant-boxes/debian/chef-11.4.0/debian-6.0.7-amd64.box"
    debi6.vm.provision :shell, :path => "install_ubuntu.sh"
  end 

  #debian 7 x64
  config.vm.define "debi7" do |debi7|32
    debi7.vm.box = "debi7_i386"
    debi7.vm.box_url = "https://s3-eu-west-1.amazonaws.com/ffuenf-vagrant-boxes/debian/debian-7.1.0-amd64.box"
    debi7.vm.provision :shell, :path => "install_debi7.sh"
  end 

  #debian 7 x86
  config.vm.define "debi7_32" do |debi7_32|
    debi7_32.vm.box = "debi7_i386"
    debi7_32.vm.box_url = "https://s3-eu-west-1.amazonaws.com/ffuenf-vagrant-boxes/debian/debian-7.1.0-amd64.box"
    debi7_32.vm.provision :shell, :path => "install_debi7.sh"
  end 

end
