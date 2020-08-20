#!/usr/bin/env bash

# find openffi install path by following /usr/local/bin/openffi symbolic link
install_path=$(readlink -f /usr/local/lib/xllr.dylib)
install_path=$(dirname $install_path)

# unlink and delete every plugin (xllr.*.dylib and openffi.compiler.*.dylib)
for plugin in "$install_path"/xllr.*.dylib;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/local/lib/$plugin_name
done

for plugin in "$install_path"/openffi.compiler.*.dylib;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/local/lib/$plugin_name
done

# unlink and delete xllr.dylib and openffi
sudo rm $install_path/xllr.dylib
sudo rm /usr/local/lib/xllr.dylib
sudo rm $install_path/openffi
sudo rm /usr/local/bin/openffi
sudo rm $install_path/uninstall.sh

# delete install path (only if empty)
sudo rmdir $install_path

echo OpenFFI uninstalled successfully!