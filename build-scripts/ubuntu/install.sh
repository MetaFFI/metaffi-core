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
# If not set, using /usr/lib/metaffi/
install_path=$1
if [ -z "$install_path" ]; then
	install_path="/usr/lib/metaffi"
fi

echo ---=== Installing MetaFFI to $install_path ===---

echo Notice: This script is using sudo - password will be prompt

sudo mkdir -p $install_path
if [ $? != 0 ]; then
	echo Failed creating $install_path directory
	exit 1
fi

echo ---=== Copying files to $install_path ===---

# copy binaries to $install_path
copy_to_install_path "*.so"
copy_to_install_path metaffi
copy_to_install_path uninstall.sh

echo ---=== Linking files from $install_path to /usr/lib and /usr/bin ===---

# place symbolic link in /usr/bin
link_files $install_path/metaffi /usr/bin/metaffi

# place symbolic link in /usr/lib
link_files $install_path/xllr.so /usr/lib/xllr.so

## following will be deprecated once "metaffi --install" option is implemented
link_files $install_path/xllr.go.so /usr/lib/xllr.go.so
link_files $install_path/xllr.python3.so /usr/lib/xllr.python3.so
link_files $install_path/metaffi.compiler.go.so /usr/lib/metaffi.compiler.go.so
link_files $install_path/metaffi.compiler.python3.so /usr/lib/metaffi.compiler.python3.so

echo ---=== MetaFFI installed successfully! ===---



