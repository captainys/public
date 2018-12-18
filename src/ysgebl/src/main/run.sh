#!/bin/bash


osid=`uname`



if [[ "$osid" == 'Darwin' ]]; then
	BUILDDIR=~/ysbin/build64
	TARGET_NAME=ysgebl64
	EXECUTABLE=~/ysbin/build64/exe/PolygonCrest/ysgebl64.app/Contents/MacOS/ysgebl64
elif [[ "$osid" == 'Linux' ]]; then
	BUILDDIR=~/ysbin/build64
	TARGET_NAME=ysgebl64
	EXECUTABLE=~/ysbin/build64/exe/PolygonCrest/ysgebl64
else
	echo Cannot identify OS type
	exit 1
fi



if [ -e ~/bin/cmake-3.1.0-rc3-Linux-x86_64/bin/cmake ]; then
	echo Downloaded cmake
	CMAKE=~/bin/cmake-3.1.0-rc3-Linux-x86_64/bin/cmake
else
	echo Default cmake
	CMAKE=cmake
fi


pushd $BUILDDIR
if [ $? -eq 0 ]; then
	echo In $BUILDDIR
else
	exit 1
fi



make $TARGET_NAME
if [ $? -eq 0 ]; then
	echo "Build Successful (run.sh)"
else
	exit 1
fi


popd



$EXECUTABLE $*
