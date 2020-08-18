function build
{
	

<#
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
#>
}

function build_installer_package
{
<#
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
	## xllr.so to output
	mkdir -p output
	cp $output_dir/xllr.so output/

	## xllr plugins to output/ - deprecated after "openffi -install" command becomes available
	cp $output_dir/xllr.go.so output/
	cp $output_dir/xllr.python3.so output/

	## openffi executable output/
	mkdir -p output/
	cp $output_dir/openffi output/

	## compiler plugins output/plugins/ - deprecated after "openffi -install" command becomes available
	cp $output_dir/openffi.compiler.go.so output/
	cp $output_dir/openffi.compiler.python3.so output/

	## copy install.sh script to output/
	cp install.sh output/

	## copy uninstall.sh script to output/
	cp uninstall.sh output/

	# zip output directory
	tar -zcvf openffi-$version.tar.gz output/

	# done
	echo Done. Version is ready in openffi-$version.tar.gz
#>
}

function install
{
<#
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
#>
}

function uninstall
{
<#
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
#>
}