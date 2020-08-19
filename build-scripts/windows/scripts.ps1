$global:output_dir = $null

function build([String] $buildtype, [String] $local_cmake_toolchain_file)
{
	# get buildtype
	if( !$buildtype )
	{
		Write-Output "Usage: build.sh build-type [cmake toolchain file]"
		Write-Output "Supported build-types: Debug, Release, RelWithDebInfo"
		Write-Output "CMake Toolchain file: Set from optional parameter or CMAKE_TOOLCHAIN_FILE environment variable"
		Exit(1)
	}

	$buildtype = $buildtype.ToLower()
	if( ($buildtype -ne "debug") -and ($buildtype -ne "release") -and ($buildtype -ne "relwithdebinfo") )
	{
		Write-Output "Usage: build.sh build-type [cmake toolchain file]"
		Write-Output "Supported build-types: Debug, Release, RelWithDebInfo"
		Write-Output "CMake Toolchain file: Set from optional parameter or CMAKE_TOOLCHAIN_FILE environment variable"
		Exit(1)
	}

	# get CMAKE_TOOLCHAIN_FILE
	if ( !$local_cmake_toolchain_file )
	{
		# if $local_cmake_toolchain_file is not set, maybe $CMAKE_TOOLCHAIN_FILE is
		local_cmake_toolchain_file=$env:CMAKE_TOOLCHAIN_FILE
	}

	# change directory to get to the root dir
	while(!(Test-Path .git -PathType Container))
	{
		Set-Location ..
	}

	# run cmake
	if ( !$local_cmake_toolchain_file )
	{
		Write-Output "cmake -DCMAKE_BUILD_TYPE=$buildtype -B output-$buildtype ."
		cmake -DCMAKE_BUILD_TYPE=$buildtype -B output-$buildtype .
	}
	else
	{
		Write-Output "cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE=$local_cmake_toolchain_file -B output-$buildtype ."
		cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE=$local_cmake_toolchain_file -B output-$buildtype .
	}

	# build
	Set-Location output-$buildtype
	make

	$global:output_dir = "output-$buildtype"
}

function build_installer_package([String] $version)
{
	# $version - semantic version (example v1.0.0)

	if( !$version )
	{
		Write-Output "Usage: build_installer_package.sh version"
		Exit(1)
	}

	# build
	build "relwithdebinfo"

	if( !$global:output_dir )
	{
		Write-Output "Something is wrong... output_dir is empty..."
		Exit(1)
	}

	# copy to "output" dir:

	## xllr.so to output
	mkdir -p output
	Copy-Item $global:output_dir/xllr.so output/

	## xllr plugins to output/ - deprecated after "openffi -install" command becomes available
	Copy-Item $global:output_dir/xllr.go.so output/
	Copy-Item $global:output_dir/xllr.python3.so output/

	## openffi executable output/
	Copy-Item $global:output_dir/openffi output/

	## compiler plugins output/plugins/ - deprecated after "openffi -install" command becomes available
	Copy-Item $global:output_dir/openffi.compiler.go.so output/
	Copy-Item $global:output_dir/openffi.compiler.python3.so output/

	## copy install scripts to output/
	Copy-Item install.bat output/
	Copy-Item uninstall.bat output/
	Copy-Item scripts.ps1 output/

	# zip output directory
	compress-archive -path "output" -destinationpath "openffi-$version.zip" -compressionlevel optimal

	Write-Output "Done. Version is ready in openffi-$version.zip"
}

function install([String] $install_path)
{
	# Verify admin rights
	$user = [Security.Principal.WindowsIdentity]::GetCurrent();
	if (! (New-Object Security.Principal.WindowsPrincipal $user).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator))
	{
		Write-Host "Installing OpenFFI requires administrative rights. Please rerun with as administrator"
		Exit(1)
	}

	# $1 (optional) - install path.
	# If not set, using %ProgramFiles%\OpenFFI
	if ( !$install_path )
	{
		install_path="$env:ProgramFiles/OpenFFI"
	}

	mkdir -p $install_path

	# copy binaries to $install_path
	Copy-Item *.so $install_path
	Copy-Item openffi $install_path
	Copy-Item uninstall.bat $install_path
	Copy-Item scripts.ps1 $install_path

	# Add $install_path to Path environment variable
	$syspath = (Get-ItemProperty -Path ‘Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment’ -Name PATH).path
	$syspath = "$syspath;$install_path"
	Set-ItemProperty -Path ‘Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment’ -Name PATH -Value $syspath

	Write-Host OpenFFI installed successfully!

}

function uninstall()
{
	# get install path
	$install_path=(Get-Item .).DirectoryName

	# remove install path from system PATH
	$syspath = (Get-ItemProperty -Path ‘Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment’ -Name PATH).path
	$syspath = "$syspath;$install_path"
	Set-ItemProperty -Path ‘Registry::HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\Session Manager\Environment’ -Name PATH -Value $syspath

	Remove-Item xllr.*.so, openffi.compiler.*.so, xllr.so, openffi, uninstall.bat, scripts.ps1

	rmdir $install_path

	Write-Host OpenFFI uninstalled successfully!

}