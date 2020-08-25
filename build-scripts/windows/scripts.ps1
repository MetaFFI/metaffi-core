$global:output_dir = $null

function assertVSToolsFound()
{
	if(! [bool](Get-Command -Name "cl" -ErrorAction SilentlyContinue))
	{
		Write-Host "cl.exe was not found - make sure you're running from Visual Studio command prompt"
		Exit(1)
	}
}

function build([String] $buildtype, [String] $local_cmake_toolchain_file)
{
	assertVSToolsFound

	$ErrorActionPreference = "Stop"

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
	if ( (!$local_cmake_toolchain_file) -and ($env:CMAKE_TOOLCHAIN_FILE) )
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
		Write-Host "cmake -DCMAKE_BUILD_TYPE=$buildtype -G "CodeBlocks - NMake Makefiles" -B output-$buildtype ."
		cmake -DCMAKE_BUILD_TYPE=$buildtype -G "CodeBlocks - NMake Makefiles" -B output-$buildtype .
	}
	else
	{
		Write-Host "cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE=$local_cmake_toolchain_file -G "CodeBlocks - NMake Makefiles" -B output-$buildtype ."
		cmake -DCMAKE_BUILD_TYPE=$buildtype -DCMAKE_TOOLCHAIN_FILE=$local_cmake_toolchain_file -G "CodeBlocks - NMake Makefiles" -B output-$buildtype .
	}

	if( ! $? )
	{
		Write-Host "OpenFFI CMake build failed!"
		Exit(1)
	}

	# build
	Set-Location output-$buildtype
	nmake /nologo all
	if( ! $? )
	{
		Write-Host "OpenFFI build failed!"
		Exit(1)
	}

	$global:output_dir = Get-Location
}

function build_installer_package([String] $version)
{
	$ErrorActionPreference = "Stop"

	# $version - semantic version (example v1.0.0)

	if( !$version )
	{
		Write-Output "Usage: build_installer_package.bat version"
		Exit(1)
	}

	$outputDir = Get-Location

	Write-Host "---=== Building version $version ===---"

	# build
	build "relwithdebinfo"

	if( !$global:output_dir )
	{
		Write-Output "Something is wrong... output_dir is empty..."
		Exit(1)
	}

	Set-Location $outputDir

	# copy to "output" dir:

	## xllr.dll to output

	If (!(Test-Path output))
	{
		mkdir -p output > $null
	}

	if( ! $? )
	{
		Write-Host "Failed to create 'output' directory"
		Exit(1)
	}

	Write-Host "---=== Copying files to installation path ===---"

	Copy-Item $global:output_dir/xllr.dll output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	## xllr plugins to output/ - deprecated after "openffi -install" command becomes available
	Copy-Item $global:output_dir/xllr.go.dll output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	Copy-Item $global:output_dir/xllr.python3.dll output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	## openffi executable output/
	Copy-Item $global:output_dir/openffi.exe output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	## compiler plugins output/plugins/ - deprecated after "openffi -install" command becomes available
	Copy-Item $global:output_dir/openffi.compiler.go.dll output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	Copy-Item $global:output_dir/openffi.compiler.python3.dll output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	## copy install scripts to output/
	Copy-Item install.bat output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	Copy-Item uninstall.bat output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	Copy-Item scripts.ps1 output/
	if( ! $? )
	{
		Write-Host "Failed to copy OpenFFI files to $global:output_dir"
		Exit(1)
	}

	Write-Output "---=== Compressing installation path ===---"

	# zip output directory
	compress-archive -f -path "output/*" -destinationpath "openffi-$version.zip" -compressionlevel optimal
	if( ! $? )
	{
		Write-Host "Failed to compress OpenFFI installation files"
		Exit(1)
	}

	Remove-Item -Recurse -Force output

	Write-Output "Done. Version is ready in openffi-$version.zip"
}

function install([String] $install_path)
{
	$ErrorActionPreference = "Stop"

	# Verify admin rights
	$user = [Security.Principal.WindowsIdentity]::GetCurrent();
	if (! (New-Object Security.Principal.WindowsPrincipal $user).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator))
	{
		Write-Host "Installing OpenFFI requires administrative rights. Please run as administrator."
		Exit(1)
	}



	# $1 (optional) - install path.
	# If not set, using %ProgramFiles%\OpenFFI
	if ( !$install_path )
	{
		$install_path="$env:ProgramFiles/OpenFFI"
	}

	if( ! (Test-Path $install_path) )
	{
		mkdir -p $install_path > $null
	}

	# copy binaries to $install_path
	Copy-Item *.dll $install_path
	Copy-Item openffi.exe $install_path
	Copy-Item uninstall.bat $install_path
	Copy-Item scripts.ps1 $install_path

	# Add $install_path to Path environment variable
	$install_path_for_env_var=$install_path -replace "/", "\"
	$syspath = [Environment]::GetEnvironmentVariable("Path", "Machine")
	if (!$syspath) # Make sure syspath is not empty to prevent any accidents :-)
	{
		Write-Host "Failed to get Path environment variable"
		Exit(1)
	}

	if( !($syspath -contains $install_path_for_env_var) )
	{
		$syspath = "$syspath;$install_path_for_env_var"
		[Environment]::SetEnvironmentVariable("Path", $syspath, "Machine")
	}

	Write-Host OpenFFI installed successfully!

}

function uninstall()
{
	# Verify admin rights
	$user = [Security.Principal.WindowsIdentity]::GetCurrent();
	if (! (New-Object Security.Principal.WindowsPrincipal $user).IsInRole([Security.Principal.WindowsBuiltinRole]::Administrator))
	{
		Write-Host "Installing OpenFFI requires administrative rights. Please run as administrator."
		Exit(1)
	}

	$ErrorActionPreference = "Stop"

	# get install path
	$install_path=(Split-Path -Path (Get-Command openffi.exe).Path)
	if (!$install_path) # Make sure install path is not empty to prevent any accidents :-)
	{
		Write-Host "Failed to get openffi installation path"
		Exit(1)
	}

	# remove install path from system PATH
	$syspath = [Environment]::GetEnvironmentVariable("Path", "Machine")
	if (!$syspath) # Make sure syspath is not empty to prevent any accidents :-)
	{
		Write-Host "Failed to get Path environment variable"
		Exit(1)
	}

	$syspath = $syspath.Replace(";$install_path", "")
	[Environment]::SetEnvironmentVariable("Path", $syspath, "Machine")

	Remove-Item $install_path\xllr.*.dll, $install_path\openffi.compiler.*.dll, $install_path\xllr.dll, $install_path\openffi.exe, $install_path\uninstall.bat, $install_path\scripts.ps1

	cd $env:ProgramFiles

	# delete directory only if empty
	if((Get-ChildItem $install_path | Measure-Object).count -eq 0)
	{
		rmdir $install_path
	}

	Write-Host OpenFFI uninstalled successfully!

	Exit(0)
}