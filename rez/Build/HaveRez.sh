#!/bin/sh

rez > /dev/null 2> /dev/null
RC=$?

if [ $RC -le 1 ] 
then
	echo "yes"
else
	echo "no"
fi

