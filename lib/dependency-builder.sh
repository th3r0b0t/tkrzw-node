#!/usr/bin/bash

printf "Current working dir should be inside lib, confirm it: \033[0;31m$(pwd)\n\033[0mIf it's not, use ctrl+c to stop script:\n"
read -p "Enter to continue or ctrl+c to stop:    "

INSTALL_PREFIX=$(pwd)

[ ! -e sources/ ] || rm -rf sources/
[ ! -e include/ ] || rm -rf include/
[ ! -e libtkrzw.a ] || rm -f libtkrzw.a

mkdir sources
cd sources
wget https://dbmx.net/tkrzw/pkg/tkrzw-1.0.32.tar.gz
tar xvf tkrzw-1.0.32.tar.gz
rm -f tkrzw-1.0.32.tar.gz

clear

printf "We need sudo rights to install liblz4-dev and cmake, please provide password if prompted:\n"
sudo apt install liblz4-dev cmake

printf "\033[0;31mWe are going to build Tkrzw library, it can take several minutes;\033[0m If you aren't ready, use ctrl+c to stop script:\n"
read -p "Enter to start building or ctrl+c to stop:    "

cd tkrzw-1.0.32
./configure --enable-opt-native --enable-most-features --enable-lz4 --disable-shared --enable-static --prefix=$INSTALL_PREFIX
make
make install

cd $INSTALL_PREFIX
mv lib/libtkrzw.a ./
rm -rf sources
rm -rf lib
rm -rf bin
