#!/bin/sh

cd bsl
make
cp bsl /boot/home/non-packaged/bin
cd ../rez
make
cp rez /boot/home/non-packaged/bin
cd ../sum-it
make

