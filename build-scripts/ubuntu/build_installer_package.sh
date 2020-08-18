#!/usr/bin/env bash

# $1 - semantic version (example v1.0.0)

if [ -z "$1" ]; then
	echo Usage: build_installer_package.sh version
	exit 1
fi

version=$1

# build binaries for installer
# $output_dir is set after build.sh is executed
./build.sh relwithdebinfo

if [ -z "$output_dir" ]; then
	echo Something went wrong... output_dir variable was expected to be set.
	exit 1
fi

# copy to "output" dir:
## xllr.so to output/xllr/
mkdir -p output/xllr/
cp $output_dir/xllr.so output/xllr/

## xllr plugins to output/xllr/ - deprecated after "openffi -install" command becomes available
cp $output_dir/xllr.go.so output/xllr/
cp $output_dir/xllr.python3.so output/xllr/

## openffi executable output/cli/
mkdir -p output/cli/
cp $output_dir/openffi output/cli/

## compiler plugins output/cli/plugins/ - deprecated after "openffi -install" command becomes available
cp $output_dir/openffi.compiler.go.so output/cli/
cp $output_dir/openffi.compiler.python3.so output/cli/

## copy install.sh script to output/
cp install.sh output/

## copy uninstall.sh script to output/
cp uninstall.sh output/

# zip output directory
tar -zcvf openffi-$version.tar.gz output/

# done
echo Done. Version is ready in openffi-$version.tar.gz