#!/usr/bin/env bash

# find metaffi install path by following /usr/bin/metaffi symbolic link
install_path=$(readlink -f /usr/lib/xllr.so)
install_path=$(dirname $install_path)

# unlink and delete every plugin (xllr.*.so and metaffi.compiler.*.so)
for plugin in "$install_path"/xllr.*.so;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/lib/$plugin_name
done

for plugin in "$install_path"/metaffi.compiler.*.so;
do
	sudo rm $plugin
	plugin_name=$(basename $plugin)
	sudo rm -f /usr/lib/$plugin_name
done

# unlink and delete xllr.so and metaffi
sudo rm $install_path/xllr.so
sudo rm /usr/lib/xllr.so
sudo rm $install_path/metaffi
sudo rm /usr/bin/metaffi
sudo rm $install_path/uninstall.sh

# delete install path (only if empty)
sudo rmdir $install_path

echo MetaFFI uninstalled successfully!