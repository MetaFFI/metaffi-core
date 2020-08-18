#!/usr/bin/env bash

# find openffi install path by following /usr/bin/openffi symbolic link
install_path=$(readlink -f /usr/lib/xllr.so)
install_path=$(dirname $install_path)

# unlink and delete every plugin (xllr.*.so and openffi.compiler.*.so)
/home/tcs/src/github.com/GreenFuze/OpenFFI/cmake-build-debug
for plugin in "$install_path"/xllr.*.so;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm /usr/lib/$plugin_name
done

for plugin in "$install_path"/openffi.compiler.*.so;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm /usr/lib/$plugin_name
done

# unlink and delete xllr.so and openffi
sudo rm $install_path/xllr.so
sudo rm /usr/lib/xllr.so
sudo rm $install_path/openffi
sudo rm /usr/bin/openffi

# delete install path (only if empty)
sudo rmdir $install_path

echo OpenFFI uninstalled successfully!