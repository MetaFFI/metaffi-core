#!/usr/bin/env bash

# find metaffi install path by following /usr/local/bin/metaffi symbolic link
install_path=$(readlink /usr/local/lib/xllr.dylib)
if [ $? != 0 ]; then
	echo Failed to find installation path
	exit 1
fi

install_path=$(dirname $install_path)

# unlink and delete every plugin (xllr.*.dylib and metaffi.compiler.*.dylib)
for plugin in "$install_path"/xllr.*.dylib;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/local/lib/$plugin_name
done

for plugin in "$install_path"/metaffi.compiler.*.dylib;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/local/lib/$plugin_name
done

# unlink and delete xllr.dylib and metaffi
sudo rm $install_path/xllr.dylib
sudo rm /usr/local/lib/xllr.dylib
sudo rm $install_path/metaffi
sudo rm /usr/local/bin/metaffi
sudo rm $install_path/uninstall.sh

# delete install path (only if empty)
sudo rmdir $install_path

echo MetaFFI uninstalled successfully!