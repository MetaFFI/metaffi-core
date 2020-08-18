#!/usr/bin/env bash

function link_files() {
	sudo ln -s "$1" "$2"
	if [ $? != 0 ]; then
		echo Failed linking "$1" to "$2"
		exit 1
	fi
}

function copy_to_install_path() {
	sudo cp "$1" "$2"
	if [ $? != 0 ]; then
		echo Failed copying "$1" to "$2"
		exit 1
	fi
}


# $1 (optional) - install path.
# If not set, using /usr/lib/openffi/
install_path=$1
if [ -z "$install_path" ]; then
	install_path="/usr/lib/openffi"
fi

echo Notice: This script is using sudo - password will be prompt

sudo mkdir -p $install_path
if [ $? != 0 ]; then
	echo Failed creating $install_path directory
	exit 1
fi

# copy binaries to $install_path
copy_to_install_path *.so $install_path
copy_to_install_path openffi $install_path

# place symbolic link in /usr/bin
link_files $install_path/openffi /usr/bin/openffi

# place symbolic link in /usr/lib
link_files $install_path/xllr.so /usr/lib/xllr.so

## following will be deprecated once "openffi --install" option is implemented
link_files $install_path/xllr.go.so /usr/lib/xllr.go.so
link_files $install_path/xllr.python3.so /usr/lib/xllr.python3.so
link_files $install_path/openffi.compiler.go.so /usr/lib/openffi.compiler.go.so
link_files $install_path/openffi.compiler.python3.so /usr/lib/openffi.compiler.python3.so

echo OpenFFI installed successfully!



