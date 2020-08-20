#!/usr/bin/env bash

function link_files() {
	echo "Linking $1 -> $2"
	sudo ln -sf "$1" "$2"
	if [ $? != 0 ]; then
		echo Failed linking "$1" to "$2"
		exit 1
	fi
}

function copy_to_install_path() {
	echo "Copying $1 -> $install_path"
	sudo cp $1 $install_path
	if [ $? != 0 ]; then
		echo Failed copying "$1" to $install_path
		exit 1
	fi
}


# $1 (optional) - install path.
# If not set, using /usr/local/lib/openffi/
install_path=$1
if [ -z "$install_path" ]; then
	install_path="/usr/local/lib/openffi"
fi

echo ---=== Installing OpenFFI to $install_path ===---

echo Notice: This script is using sudo - password will be prompt

sudo mkdir -p $install_path
if [ $? != 0 ]; then
	echo Failed creating $install_path directory
	exit 1
fi

echo ---=== Copying files to $install_path ===---

# copy binaries to $install_path
copy_to_install_path "*.dylib"
copy_to_install_path openffi
copy_to_install_path uninstall.sh

echo ---=== Linking files from $install_path to /usr/local/lib and /usr/bin ===---

# place symbolic link in /usr/bin
link_files $install_path/openffi /usr/local/bin/openffi

# place symbolic link in /usr/local/lib
link_files $install_path/xllr.dylib /usr/local/lib/xllr.dylib

## following will be deprecated once "openffi --install" option is implemented
link_files $install_path/xllr.go.dylib /usr/local/lib/xllr.go.dylib
link_files $install_path/xllr.python3.dylib /usr/local/lib/xllr.python3.dylib
link_files $install_path/openffi.compiler.go.dylib /usr/local/lib/openffi.compiler.go.dylib
link_files $install_path/openffi.compiler.python3.dylib /usr/local/lib/openffi.compiler.python3.dylib

echo ---=== OpenFFI installed successfully! ===---



