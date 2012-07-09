#!/bin/sh

cd bsl
make
cp bsl /boot/home/config/bin
cd ../rez
make
cp rez /boot/home/config/bin
cd ../sum-it
make

