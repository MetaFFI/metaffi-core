## Build From Source
The `build-scripts` directory contains 4 scripts for each supported operating system:
* `build` - Build OpenFFI project
* `build_installer_package` - Build OpenFFI installer package
* `install` - Install script that will be placed inside the installer package
* `uninstall` - Install script that will be placed inside the installer package

In Windows the scripts are batch files calling `scripts.ps1` powershell file.\
In Ubuntu & MacOS, scripts are bash files. 

### Prerequisites
* C++ compiler must support C++17
* [CMake 3.16.0 and above](https://cmake.org/install/)
* [Boost library](https://www.boost.org/) - Specific components: 
    * thread
    * filesystem
    * program_options
* [Go](https://golang.org/dl/) - To build official plugins
* [Python3](https://www.python.org/downloads/) - To run python examples and unitests 

### Build
Run the `build` scripts in your operating system. `build` requires 2 parameters:
* (required) build-otype: Debug, Release, RelWithDebInfo
* (optional) CMake Toolchain file (optional): Set from parameter or CMAKE_TOOLCHAIN_FILE environment variable.
    * Toolchain file is required for cases you are using package management (like vcpkg) to manage your C/C++ libraries (like boost)

OpenFFI will be build to *OpenFFIRoot/output-[build-otype]* directory

<BR />

**Remarks**\
In windows, in case boost is not found during cmake build process, set the following environment variables:
* BOOST_INCLUDEDIR    [path to boost]
* BOOST_LIBRARYDIR    [path to boost]\lib64-msvc-[version]
* BOOST_ROOT          [path to boost]\boost

### Running Tests
Go to the output directory using your terminal window and run `ctest` command