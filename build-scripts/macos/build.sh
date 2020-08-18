#!/usr/bin/env bash

if [ -z "$1" ]; then
	echo Usage: build.sh build-type [cmake toolchain file]
	echo Supported build-types: Debug, Release, RelWithDebInfo
	echo CMake Toolchain file: Set from optional parameter or CMAKE_TOOLCHAIN_FILE environment variable
	exit 1
fi

# $1 - build type (Debug, Release, RelWithDebInfo)
buildtype=$(echo $1 | tr '[:upper:]' '[:lower:]')
if [ "$buildtype" != "debug" ] && [ "$buildtype" != "release" ] && [ "$buildtype" != "relwithdebinfo" ]; then
	echo Usage: build.sh build-type [cmake toolchain file]
	echo Supported build-types: Debug, Release, RelWithDebInfo
	echo CMake Toolchain file: Set from optional parameter or CMAKE_TOOLCHAIN_FILE environment variable
	exit 1
fi

# $2 - cmake toolchain (optional)
local_cmake_toolchain_file=$2
if [ -z "$local_cmake_toolchain_file" ]; then
	local_cmake_toolchain_file=$CMAKE_TOOLCHAIN_FILE # if $2 is not set, maybe $CMAKE_TOOLCHAIN_FILE is
fi

# change directory to get to the root dir
while [ ! -d .git ]
do
  cd ..
done

if [ -z "$local_cmake_toolchain_file" ]; then
	echo "cmake -DCMAKE_BUILD_TYPE=$buildtype -B output-$buildtype ."
	cmake -DCMAKE_BUILD_TYPE=$buildtype -B output-$buildtype .
else
	echo "cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE=$local_cmake_toolchain_file -B output-$buildtype ."
	cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE="$local_cmake_toolchain_file" -B output-$buildtype .
fi

if [ $? != 0 ]; then
	echo Failed building CMake
	exit 1
fi

cd output-$buildtype || (echo "cannot find output-$buildtype" && exit 1)

make
if [ $? != 0 ]; then
	echo Failed building with make
	exit 1
fi

export output_dir="output-$buildtype"