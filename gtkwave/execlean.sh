#!/bin/sh

# windows targets only

if [ `uname|grep CYGWIN|wc -l` = "1" ]
	then
	echo Removing all .exe files...
	find . | grep '.exe$' | awk '{print "rm "$0}' | sh
	fi

if [ `uname|grep MINGW|wc -l` = "1" ]
	then
	echo Removing all .exe files...
	find . | grep '.exe$' | awk '{print "rm "$0}' | sh
	fi

