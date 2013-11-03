#!/bin/sh

cd bsl
make
cp bsl /boot/home/config/non-packaged/bin
cd ../rez
make
cp rez /boot/home/config/non-packaged/bin
cd ../sum-it
make

