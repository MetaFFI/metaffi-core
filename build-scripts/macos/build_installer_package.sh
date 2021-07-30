#!/usr/bin/env bash

# $1 - semantic version (example v1.0.0)

if [ -z "$1" ]; then
	echo Usage: build_installer_package.sh version
	exit 1
fi

version=$1

echo ---=== Building version $version ===---

script_dir=$PWD

# build binaries for installer
# $output_dir is set after build.sh is executed
source ./build.sh relwithdebinfo

if [ -z "$output_dir" ]; then
	echo Something went wrong... output_dir variable was expected to be set.
	exit 1
fi

# get back to the scripts dir
cd $script_dir

echo ---=== Copying files to installation path ===---

# copy to "output" dir:
## xllr.dylib to output
mkdir -p output
cp $output_dir/xllr.dylib output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

## xllr plugins to output/ - deprecated after "metaffi -install" command becomes available
cp $output_dir/xllr.*.dylib output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

## metaffi executable output/
cp $output_dir/metaffi output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

## compiler plugins output/plugins/ - deprecated after "metaffi -install" command becomes available
cp $output_dir/metaffi.compiler.*.dylib output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

## copy install.sh script to output/
cp install.sh output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

## copy uninstall.sh script to output/
cp uninstall.sh output/
if [ $? != 0 ]; then
	echo Failed copying file
	exit 1
fi

echo ---=== Compressing installation path ===---

# zip output directory
cd output
tar -zcvf metaffi-$version.tar.gz *
if [ $? != 0 ]; then
	cd ..
	rm -R output
	echo Failed compressing output directory
	exit 1
fi
cd ..
mv output/metaffi-$version.tar.gz .
rm -R output

# done
echo ---=== Done. Version is ready in metaffi-$version.tar.gz ===---