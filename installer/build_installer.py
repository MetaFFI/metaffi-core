import zipfile
import io
import base64
import shutil
import re
from typing import List


def zip_installer_files(files: List[str], root:str):
	# Create a file-like object in memory
	buffer = io.BytesIO()
	
	# Create a zip file object and write the files to it
	# Use the highest compression level
	with zipfile.ZipFile(buffer, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as zf:
		for file in files:
			# Read the file from the filename using the parameters and value "root+file"
			# Write the file into the zip with the filename written in "file" value
			zf.write(root + file, arcname=file)
	
	# Get the byte array from the buffer
	return buffer.getvalue()


def update_python_file(python_source_filename, windows_zip, ubuntu_zip):
	# Encode the binary data to base64 strings
	windows_zip_str = base64.b64encode(windows_zip)
	ubuntu_zip_str = base64.b64encode(ubuntu_zip)

	# Open the source file in read mode
	with open(python_source_filename, "r") as f:
		# Read the source code as a string
		source_code = f.read()

	# Find and replace the variables with the encoded strings
	source_code = re.sub(r"windows_x64_zip\s*=\s*.+", f"windows_x64_zip = {windows_zip_str}", source_code, count=1)
	source_code = re.sub(r"ubuntu_x64_zip\s*=\s*.+", f"ubuntu_x64_zip = {ubuntu_zip_str}", source_code, count=1)

	# Open the source file in write mode
	with open(python_source_filename, "w") as f:
		# Write the updated source code to the file
		f.write(source_code)


def get_windows_metaffi_files():
	files = []

	# metaffi
	files.extend(['xllr.dll', 'metaffi.exe', 'bin/vcruntime140_1d.dll', 'bin/vcruntime140d.dll', 'bin/boost_filesystem-mt-gd-x64.dll', 'bin/boost_program_options-mt-gd-x64.dll', 'bin/msvcp140d.dll', 'bin/ucrtbased.dll'])

	# python plugin
	files.extend(['xllr.python3.dll'])

	# go plugin
	files.extend(['xllr.go.dll', 'metaffi.compiler.go.dll'])

	# openjdk plugin
	files.extend(['xllr.openjdk.dll', 'xllr.openjdk.bridge.jar', 'xllr.openjdk.jni.bridge.dll'])

	return files


def get_ubuntu_metaffi_files():
	files = []

	# metaffi
	files.extend(['xllr.so', 'metaffi', 'lib/libstdc++.so.6.0.30', 'lib/libc.so.6', 'lib/libboost_thread-mt-d-x64.so.1.79.0', 'lib/libboost_program_options-mt-d-x64.so.1.79.0', 'lib/libboost_filesystem-mt-d-x64.so.1.79.0'])

	# python plugin
	files.extend(['xllr.python3.so'])

	# go plugin
	files.extend(['xllr.go.so', 'metaffi.compiler.go.so'])

	# openjdk plugin
	files.extend(['xllr.openjdk.so', 'xllr.openjdk.bridge.jar', 'xllr.openjdk.jni.bridge.so'])

	return files


def main():
	windows_files = get_windows_metaffi_files()
	ubuntu_files = get_ubuntu_metaffi_files()

	windows_zip = zip_installer_files(windows_files, './../out/windows/x64/debug/')
	ubuntu_zip = zip_installer_files(ubuntu_files, './../out/ubuntu/x64/debug/')

	shutil.copy('install_metaffi_template.py', 'install_metaffi.py')

	update_python_file('install_metaffi.py', windows_zip, ubuntu_zip)
	
	print('Done')


if __name__ == '__main__':
	main()
