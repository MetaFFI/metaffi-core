import base64
import io
import platform
import re
import shlex
import shutil
import sys
import ctypes
import os
import tempfile
import traceback
import typing
import zipfile
import subprocess
import urllib.request

windows_x64_zip = 'windows_x64_zip_data'
ubuntu_x64_zip = 'ubuntu_x64_zip_data'

is_silent = False
is_skip_tests = False
is_extended_tests = False
is_run_windows_patched_go_tests = False


# ====================================

def install_pip_package(package_name: str):
	command = f"pip show {package_name}"
	err_code, stdout, stderr = run_command(command, False, False)
	
	if err_code != 0:
		reply = ask_user(f'{package_name} python package is required for the installation, do you want me to install it?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Cannot continue without {package_name}, please install it and try again')
		
		command = f"{sys.executable} -m pip install {package_name}"
		err_code, stdout, stderr = run_command(command, True, False)
		if err_code != 0:
			raise Exception(f"Failed installing {package_name} with the command {command}. Error code {err_code}. Output:\n{stdout}{stderr}")


def ask_user(input_text: str, default: str, valid_answers: list | None):
	global is_silent
	
	if is_silent:
		if default is None or default == '':
			raise Exception(f"internal error - missing default value in silent mode.\n{input_text}\n{valid_answers}")
		return default
	
	done = False
	
	msg = input_text
	if valid_answers is not None:
		msg += ' [' + '/'.join(valid_answers) + '] '
	if default is not None and default != '':
		msg += f'(default: {default}) '
	msg += ' - '
	
	answer = None
	
	while not done:
		answer = input(msg)
		answer = answer.strip()
		
		if answer == '' and default is not None and default != '':
			answer = default
		
		if valid_answers is not None:
			if answer.lower() not in [s.lower() for s in valid_answers]:
				print('Not a valid input.')
				continue
		
		done = True
	
	return answer


def is_windows():
	return platform.system() == 'Windows'


def is_ubuntu():
	if platform.system() != 'Linux':
		return False
	
	import distro
	return distro.name() == 'Ubuntu'


def is_path_string_valid(maybepath: str) -> bool:
	try:
		os.path.abspath(maybepath)
		return True
	except:
		return False


def get_install_dir(default_dir: str):
	install_dir = None
	
	# If METAFFI_HOME environment variable is set, return its value
	if "METAFFI_HOME" in os.environ:
		install_dir = os.environ["METAFFI_HOME"]
	
	# Otherwise, ask the user for the installation directory
	else:
		done = False
		while not done:
			# Ask the user for the input
			user_input = ask_user(f"Where to install. Notice due to issues in some languages, do not use whitespaces? ", default_dir, None)
			
			user_input = os.path.expanduser(os.path.expandvars(user_input))
			
			if ' ' in user_input:
				print('Installation directory mustn\'t contains whitespace')
				continue
			
			if not is_path_string_valid(user_input):
				print(f'Given path "{user_input}" is not valid')
				continue
			
			are_you_sure = ask_user(f'Are you sure you want to install to "{user_input}"?', 'y', ['y', 'n'])
			if are_you_sure.strip().lower() == 'n':
				continue
			
			install_dir = user_input
			done = True
	
	install_dir = os.path.abspath(install_dir)
	print('installing to ' + install_dir)
	return install_dir


# Define the function
def unpack_into_directory(base64_zip_file, target_directory):
	# Decode the base64 string to a byte array
	zip_data = base64.b64decode(base64_zip_file)
	
	zip_data = io.BytesIO(zip_data)  # to clear a pycharm warning
	
	# Create a zip file object from the byte array
	# Specify the compression type and level
	zf = zipfile.ZipFile(zip_data, "r", zipfile.ZIP_DEFLATED, compresslevel=9)
	
	# Check if the target directory exists
	if not os.path.exists(target_directory):
		# Create the target directory
		os.makedirs(target_directory)
	
	# Extract all the files from the zip file object to the target directory
	zf.extractall(target_directory)


refresh_env: typing.Callable


def run_command(command: str, raise_if_command_fail: bool = False, is_refresh_envvars: bool = True):
	global refresh_env
	global is_silent
	
	if is_windows():
		print(f'{os.getcwd()}> {command}')
	else:
		print(f'{os.getcwd()}$ {command}')
	
	# create a process object with the command line
	if is_refresh_envvars:
		refresh_env()
	
	try:
		command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
		
		env = os.environ.copy()
		if is_silent and not is_windows():
			env["DEBIAN_FRONTEND"] = "noninteractive"
		
		output = subprocess.run(command_split, capture_output=True, text=True, env=env)
	except subprocess.CalledProcessError as e:
		if raise_if_command_fail:
			raise Exception(f'Failed running "{command}" with exit code {e.returncode}. Output:\n{str(e.stdout)}{str(e.stderr)}')
		
		# your code to handle the exception
		return e.returncode, str(e.stdout), str(e.stderr)
	except FileNotFoundError as e:
		if raise_if_command_fail:
			raise Exception(f'Failed running {command} with {e.strerror}.\nfile: {e.filename}')
		
		return f'Failed running {command} with {e.strerror}.\nfile: {e.filename}', '', ''
	
	all_stdout = output.stdout
	all_stderr = output.stderr
	
	# if the return code is not zero, raise an exception
	return output.returncode, str(all_stdout).strip(), str(all_stderr).strip()


def run_shell(command: str, raise_if_command_fail: bool = False):
	global refresh_env
	
	if is_windows():
		shell = 'cmd.exe'
		print(f'{os.getcwd()}> {command}')
	else:
		shell = "/bin/bash"
		print(f'{os.getcwd()}$ {command}')
	
	# create a process object with the command line
	refresh_env()
	try:
		command_split = shlex.split(os.path.expanduser(os.path.expandvars(command)))
		output = subprocess.run(command_split, executable=shell, capture_output=True, text=True, shell=True)
	except subprocess.CalledProcessError as e:
		
		if raise_if_command_fail:
			raise Exception(f'Failed running "{command}" with exit code {e.returncode}. Output:\n{str(e.stdout)}{str(e.stderr)}')
		
		# your code to handle the exception
		return e.returncode, str(e.stdout), str(e.stderr)
	except FileNotFoundError as e:
		if raise_if_command_fail:
			raise f'Failed running {command} with {e.strerror}.\nfile: {e.filename}'
		
		return 1, '', f'Failed running {command} with {e.strerror}.\nfile: {e.filename}'
	
	all_stdout = str(output.stdout).strip()
	all_stderr = str(output.stderr).strip()
	
	if raise_if_command_fail and output.returncode != 0:
		raise Exception(f'Failed running "{command}" with exit code {output.returncode}. Output:\n{all_stdout}{all_stderr}')
	
	# if the return code is not zero, raise an exception
	return output.returncode, all_stdout, all_stderr


def check_go_installed(install_go: typing.Callable):
	# try to run the go version command and capture the output
	
	exit_code, stdout, stderr = run_command("go version")
	
	if exit_code != 0:
		is_install_go = ask_user("Go was not detected, do you want me to install it for you", 'y', ['y', 'n'])
		if is_install_go == 'y':
			install_go()
			refresh_env()
		else:
			raise Exception(f"Go is not installed or not in the PATH. Make sure Go is installed and try again.")
	
	exit_code, stdout, stderr = run_command("go env GOROOT")
	# check if the command was successful
	if exit_code != 0:
		msg = f"An error occurred while running 'go env GOROOT' with: '{exit_code}'. The output is:\n"
		msg += stdout + '\n'
		msg += stderr
		raise Exception(msg)
	
	goroot = stdout
	if not goroot:
		raise Exception("GOROOT is not set. Please use the 'go env -w' command to set it and try again.")
	
	exit_code, stdout, stderr = run_command("go env CGO_ENABLED")
	
	if exit_code != 0:
		msg = f"An error occurred while running 'go env CGO_ENABLED'. The output is:\n"
		msg += stderr
		raise Exception(msg)
	
	# get the value of CGO_ENABLED from the output
	cgo_enabled = stdout
	# check if it is equal to "1"
	if cgo_enabled != "1":
		print(f"MetaFFI requires CGo is to be enabled")
		is_enable = ask_user("Do you want me to enable it?", 'y', ['y', 'n'])
		if is_enable.strip().lower() == 'y':
			output = subprocess.run("go env -w CGO_ENABLED=1".split(' '), capture_output=True, text=True)
			if output.returncode != 0:
				raise Exception(f'Failed to enable CGo. Command "go env -w CGO_ENABLED=1" failed:\n{output.stderr}')
			else:
				print('Enabled CGo')
		else:
			raise Exception('Cancelling installation')


def add_metaffi_home_to_cgo_cflags(install_dir: str, set_system_env_var: typing.Callable):
	exit_code, stdout, stderr = run_command("go env CGO_CFLAGS")
	
	if exit_code != 0:
		msg = f"An error occurred while running 'go env CGO_ENABLED'. The output is:\n"
		msg += stderr
		raise Exception(msg)
	
	# get the value of CGO_ENABLED from the output
	cgo_cflags = stdout
	
	if install_dir.lower() not in cgo_cflags.lower():
		# check if it is equal to "1"
		reply = ask_user(f'MetaFFI requires to add "{install_dir}" as an include directory to CGO_CFLAGS, do want me to do that for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Set -I"{install_dir}" to CGO_CFLAGS and try again')
		
		# if cgo_cflags is not None and cgo_cflags != '':
		# 	new_cflags = f'"{cgo_cflags} -I{install_dir}"'
		# 	set_system_env_var('CGO_CFLAGS', f'{new_cflags}')
		# else:
		set_system_env_var('CGO_CFLAGS', f'-I{install_dir}')
		
		refresh_env()


def install_windows_gcc():
	# Check if gcc.exe exists
	gcc_path = shutil.which("gcc.exe")
	
	if gcc_path is not None:  # gcc installed
		return
	
	reply = ask_user('gcc, which is required for CGo, is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
	if reply == 'n':
		raise Exception('Please install gcc and try again')
	
	url = "https://github.com/jmeubank/tdm-gcc/releases/download/v10.3.0-tdm64-2/tdm64-gcc-10.3.0-2.exe"
	
	# Download the exe file from the given URL
	print(f'Downloading and installing {url}')
	
	file_name = url.split("/")[-1]
	temp_dir = os.environ["TEMP"]
	file_path = os.path.join(temp_dir, file_name)
	
	# Use urllib.request.urlretrieve to download the file
	urllib.request.urlretrieve(url, file_path)
	
	exit_code, stdout, stderr = run_command(file_path.replace("\\", '/'))
	# os.remove(file_path)
	
	refresh_env()
	
	if exit_code != 0:
		raise Exception(f'Failed to install gcc. Error: {exit_code}.\nstdout: {stdout}\nstderr: {stderr}')
	
	return


def python_exe() -> str:
	if is_windows():
		return 'python'
	else:
		return shutil.which('python3.11')


# ========== unitests ==========

def get_exe_format(execname):
	if platform.system() == 'Windows':
		return f'{execname}.exe'
	else:
		return f'./{execname}'


def metaffi_go_guest(path: str, source: str):
	os.chdir(path)
	err_code, stdout, stderr = run_command(f'metaffi -c --idl {source} -g')
	if err_code != 0:
		raise Exception(f'Failed to build MetaFFI guest for {path}/{source}. Output:\n{stdout}{stderr}')


def run_python_file(path: str):
	with open("my_script.py") as f:
		code = f.read()
		exec(code)


def run_go_tests():
	def run(path: str, exec_name: str):
		os.chdir(path)
		
		err_code, stdout, stderr = run_command('go get -u')
		if err_code != 0:
			raise Exception(f'Failed "go get -u" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command('go build')
		if err_code != 0:
			raise Exception(f'Failed "go build" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command(exec_name)
		os.remove(exec_name)
		if err_code != 0:
			raise Exception(f'sanity failed in {path}.\n{stdout}{stderr}')
	
	print('Go --> Python3.11')
	run(os.environ['METAFFI_HOME'] + '/tests/go/sanity/python3', get_exe_format('python3'))
	
	if not is_windows() or is_run_windows_patched_go_tests:
		print('Go --> OpenJDK')
		run(os.environ['METAFFI_HOME'] + '/tests/go/sanity/openjdk', get_exe_format('openjdk'))
	else:
		print('Skipping Go --> OpenJDK (Go bug not patched on Windows)')


def run_extended_go_tests():
	def run(path: str, exec_name: str):
		os.chdir(path)
		
		err_code, stdout, stderr = run_command('go get -u')
		if err_code != 0:
			raise Exception(f'Failed "go get -u" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command('go build')
		if err_code != 0:
			raise Exception(f'Failed "go build" in path {path}.\n{stdout}{stderr}')
		
		err_code, stdout, stderr = run_command(exec_name)
		os.remove(exec_name)
		if err_code != 0:
			raise Exception(f'sanity failed in {path}.\n{stdout}{stderr}')
	
	print('Go --> Python3.11 (BeautifulSoup)')
	install_pip_package('bs4')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/beautifulsoup/', get_exe_format('beautifulsoup'))
	
	print('Go --> Python3.11 (Collections)')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/collections/', get_exe_format('collections'))
	
	print('Go --> Python3.11 (Named, positional and variant arguments)')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/complex-primitives/', get_exe_format('complex-primitives'))
	
	print('Go --> Python3.11 (numpy)')
	install_pip_package('numpy')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/numpy/', get_exe_format('numpy'))
	
	print('Go --> Python3.11 (pandas)')
	install_pip_package('pandas')
	run(os.environ['METAFFI_HOME'] + '/tests/go/extended/python3/pandas/', get_exe_format('pandas'))
	
	if not is_windows() or is_run_windows_patched_go_tests:
		print('Go --> OpenJDK')
		run(os.environ['METAFFI_HOME'] + '/tests/go/extended/openjdk/log4j/', get_exe_format('log4j'))
	else:
		print('Skipping Go --> OpenJDK (Go bug not patched on Windows)')


def run_openjdk_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		metaffi_home = os.environ['METAFFI_HOME'] + '/'
		openjdk_api = metaffi_home + 'metaffi.api.jar'
		bridge = metaffi_home + 'xllr.openjdk.bridge.jar'
		junit = metaffi_home + '/tests/openjdk/junit-platform-console-standalone-1.10.2.jar'
		hamcrest = metaffi_home + '/tests/openjdk/hamcrest-core-1.3.jar'
		
		error_code, stdout, stderr = run_command(f'javac -cp "{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{junit}{os.pathsep}{hamcrest}" {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed compiling openjdk test:\n{stdout}{stderr}')
		
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'java -jar "{junit}" -cp ".{os.pathsep}{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{hamcrest}" -d. --select-class {test_file} --details=Verbose --fail-if-no-tests --disable-banner')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code} openjdk sanity test:\n{stdout}{stderr}')
	
	print('OpenJDK --> Python3.11')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/', 'APITestPython3.java')
	
	print('OpenJDK --> Go')
	metaffi_go_guest(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/go', 'TestRuntime.go')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/sanity/', 'APITestGo.java')


def run_extended_openjdk_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		metaffi_home = os.environ['METAFFI_HOME'] + '/'
		openjdk_api = metaffi_home + 'metaffi.api.jar'
		bridge = metaffi_home + 'xllr.openjdk.bridge.jar'
		junit = metaffi_home + '/tests/openjdk/junit-platform-console-standalone-1.10.2.jar'
		hamcrest = metaffi_home + '/tests/openjdk/hamcrest-core-1.3.jar'
		
		error_code, stdout, stderr = run_command(f'javac -cp "{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{junit}{os.pathsep}{hamcrest}" {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed compiling openjdk test:\n{stdout}{stderr}')
		
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'java -jar "{junit}" -cp ".{os.pathsep}{openjdk_api}{os.pathsep}{bridge}{os.pathsep}{hamcrest}" -d. --select-class {test_file} --details=Verbose --fail-if-no-tests --disable-banner')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code} openjdk sanity test:\n{stdout}{stderr}')
	
	print('OpenJDK --> Python3.11 (BeautifulSoup)')
	install_pip_package('bs4')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/beautifulsoup/', 'BeautifulSoupTest.java')
	
	print('OpenJDK --> Python3.11 (Collections)')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/collections/', 'CollectionsTest.java')
	
	print('OpenJDK --> Python3.11 (Named, positional and variant arguments)')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/complex-primitives/', 'ComplexPrimitivesTest.java')
	
	print('OpenJDK --> Python3.11 (numpy)')
	install_pip_package('numpy')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/numpy/', 'NumpyTest.java')
	
	print('OpenJDK --> Python3.11 (pandas)')
	install_pip_package('pandas')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/python3/pandas/', 'PandasTest.java')
	
	print('OpenJDK --> Go (gomcache)')
	run_python_file(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/go/gomcache/build_metaffi.py')
	run(os.environ['METAFFI_HOME'] + '/tests/openjdk/extended/go/gomcache/', 'GoMCacheTest.java')


def run_python_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'{python_exe()} -m unittest {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code}:\n{stdout}{stderr}')
	
	print('Python3.11 --> OpenJDK')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/openjdk/', 'openjdk_test.py')
	
	print('Python3.11 --> Go')
	metaffi_go_guest(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/go/', 'TestRuntime.go')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/sanity/go/', 'go_test.py')


def run_extended_python_tests():
	def run(path: str, test_file: str):
		os.chdir(path)
		
		# build
		test_file = os.path.splitext(test_file)[0]
		error_code, stdout, stderr = run_command(f'{python_exe()} -m unittest {test_file}')
		
		if error_code != 0:
			raise Exception(f'Failed {test_file} with {error_code}:\n{stdout}{stderr}')
	
	print('Python3.11 --> OpenJDK (log4j)')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/extended/openjdk/log4j/', 'log4j_test.py')
	
	print('Python3.11 --> Go (gomcache)')
	run_python_file(os.environ['METAFFI_HOME'] + '/tests/python3/extended/go/gomcache/build_metaffi.py')
	run(os.environ['METAFFI_HOME'] + '/tests/python3/extended/go/gomcache/', 'gomcache_test.py')


def run_sanity_tests():
	run_python_tests()
	run_openjdk_tests()
	run_go_tests()


def run_extended_tests():
	run_extended_python_tests()
	run_extended_openjdk_tests()
	run_extended_go_tests()


def install_python3_api():
	answer = ask_user('Do you want to install MetaFFI Python3 API?', 'y', ['y', 'n'])
	
	if answer.strip() == 'y':
		err_code, stdout, stderr = run_command(f"{python_exe()} -m pip install metaffi-api")
		if err_code != 0:
			raise Exception(f'Failed to install metaffi-api python package (error code: {err_code}):\n{stderr}{stdout}')


def install_apis():
	install_python3_api()


# ========== windows ===========


def set_windows_system_environment_variable(name: str, val: str):
	import winreg
	
	# already set correctly, no need to set
	if name in os.environ and val == os.environ[name]:
		return
	
	# Open the registry key for system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, winreg.KEY_ALL_ACCESS)
	# Set the value of the environment variable
	# Use REG_EXPAND_SZ as the type to allow references to other variables
	winreg.SetValueEx(key, name, 0, winreg.REG_EXPAND_SZ, val)
	# Close the key
	winreg.CloseKey(key)
	# Notify the system that the environment variables have changed
	# Use HWND_BROADCAST and WM_SETTINGCHANGE messages
	ctypes.windll.user32.SendMessageW(0xFFFF, 0x001A, 0, "Environment")
	
	print(f'Setting system-wide environment variable {name} to {val}')


def set_windows_user_environment_variable(name: str, val: str):
	import winreg
	
	# open the registry key for the current user's environment variables
	key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", access=winreg.KEY_WRITE)
	
	# set the value of the environment variable in the registry
	winreg.SetValueEx(key, name, 0, winreg.REG_SZ, val)
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))


# Define the function
def add_to_path_environment_variable(path):
	import winreg
	
	# Open the registry key for system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, winreg.KEY_ALL_ACCESS)
	
	# Get the current value of the Path variable
	current_path = winreg.QueryValueEx(key, "Path")[0]
	
	# Check if the given path is already in the Path variable
	if not any(os.path.samefile(path, os.path.expanduser(os.path.expandvars(p))) for p in current_path.split(";")):
		# Append the given path to the Path variable
		new_path = current_path + ";" + path
		
		# Set the new value of the Path variable
		# Use REG_EXPAND_SZ as the type to allow references to other variables
		winreg.SetValueEx(key, "Path", 0, winreg.REG_EXPAND_SZ, new_path)
		
		# Notify the system that the environment variables have changed
		# Use HWND_BROADCAST and WM_SETTINGCHANGE messages
		ctypes.windll.user32.SendMessageW(0xFFFF, 0x001A, 0, "Environment")
		
		print(f'Adding {path} to PATH environment variable')


def get_dll_version(filename):
	import ctypes
	# Define the VS_FIXEDFILEINFO structure
	class VS_FIXEDFILEINFO(ctypes.Structure):
		_fields_ = [
			("dwSignature", ctypes.c_ulong),
			("dwStrucVersion", ctypes.c_ulong),
			("dwFileVersionMS", ctypes.c_ulong),
			("dwFileVersionLS", ctypes.c_ulong),
			("dwProductVersionMS", ctypes.c_ulong),
			("dwProductVersionLS", ctypes.c_ulong),
			("dwFileFlagsMask", ctypes.c_ulong),
			("dwFileFlags", ctypes.c_ulong),
			("dwFileOS", ctypes.c_ulong),
			("dwFileType", ctypes.c_ulong),
			("dwFileSubtype", ctypes.c_ulong),
			("dwFileDateMS", ctypes.c_ulong),
			("dwFileDateLS", ctypes.c_ulong),
		]
	
	# Load the version.dll library
	version = ctypes.windll.version
	# Get the size of the file version info
	size = version.GetFileVersionInfoSizeA(filename.encode('ascii'), None)
	if size == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Allocate a buffer for the file version info
	buffer = ctypes.create_string_buffer(size)
	# Get the file version info
	res = version.GetFileVersionInfoA(filename.encode('ascii'), None, size, buffer)
	if res == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Get a pointer to the VS_FIXEDFILEINFO structure
	ffi = ctypes.POINTER(VS_FIXEDFILEINFO)()
	pLen = ctypes.c_uint()
	# Query the value of the VS_FIXEDFILEINFO structure
	res = version.VerQueryValueA(buffer, "\\", ctypes.byref(ffi), ctypes.byref(pLen))
	if res == 0:
		# Get the last error code
		err_code = ctypes.get_last_error()
		# Get the error message
		err_msg = ctypes.FormatError(err_code)
		# Raise a WindowsError exception
		raise WindowsError(err_code, err_msg)
	# Extract the file version from the VS_FIXEDFILEINFO structure
	ms = ffi.contents.dwFileVersionMS
	ls = ffi.contents.dwFileVersionLS
	return f'{ms >> 16}.{ms & 0xFFFF}.{ls >> 16}.{ls & 0xFFFF}'


def install_windows_python():
	return_code, all_stdout, all_stderr = run_command('winget install -e --silent --accept-source-agreements --accept-package-agreements --id Python.Python.3.11')
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception('Failed to install Python 3.11')


def install_windows_openjdk(version):
	return_code, all_stdout, all_stderr = run_command(f'winget install -e --silent --accept-source-agreements --accept-package-agreements --id Microsoft.OpenJDK.{version}')
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception('Failed to install OpenJDK')


def install_windows_go():
	refresh_env()
	return_code, all_stdout, all_stderr = run_command('winget install -e --silent --accept-source-agreements --accept-package-agreements --id GoLang.Go')
	print(os.environ['PATH'])
	if return_code != 0:
		if 'Found an existing package already installed.' not in all_stdout and 'Found an existing package already installed.' not in all_stderr:
			raise Exception(f'Failed to install Go. returned with: {return_code}.\nstdout: {all_stdout}\nstderr: {all_stderr}')


def check_python_windows_installed(version: str):
	from ctypes import wintypes
	
	# split the version number into major, minor, and micro parts
	major, minor = map(int, version.split("."))
	
	# construct the DLL file name based on the version number
	dll_name = f"python{major}{minor}.dll"
	
	# try to load the DLL file using ctypes
	try:
		dll = ctypes.WinDLL(dll_name)
	except OSError:
		is_install_python = ask_user("Python3.11 was not detected, do you want me to install it for you?", 'y', ['y', 'n'])
		if is_install_python == 'y':
			install_windows_python()
		else:
			# if the DLL file cannot be loaded, print an error message
			raise Exception(f"{dll_name} cannot be loaded. Please check your Python {version} is installed, and in PATH environment variable")
	
	# Define the GetModuleFileName function
	kernel32 = ctypes.WinDLL('kernel32', use_last_error=True)
	kernel32.GetModuleFileNameW.restype = wintypes.DWORD
	kernel32.GetModuleFileNameW.argtypes = (wintypes.HMODULE, wintypes.LPWSTR, wintypes.DWORD)
	# Call the GetModuleFileName function with the DLL handle
	buffer = ctypes.create_unicode_buffer(wintypes.MAX_PATH)
	length = kernel32.GetModuleFileNameW(dll._handle, buffer, len(buffer))
	if length == 0:
		raise ctypes.WinError(ctypes.get_last_error())
	
	dll_version = get_dll_version(buffer.value)
	
	# compare the DLL version with the expected version
	if not dll_version.startswith(version):
		# if they don't match, print an error message
		raise Exception(f"Python was found {dll_name}, but with version {dll_version}, which does not match the expected version {version}. Please install the supported Python version.")


def check_windows_pythonhome(version: str):
	from ctypes import util
	
	major, minor = map(int, version.split("."))
	
	# construct the DLL file name based on the Python version
	dll_name = f"python{major}{minor}.dll"
	
	# try to get the full path of the DLL file using ctypes
	try:
		dll_path = util.find_library(dll_name)
	except OSError:
		raise Exception(f"{dll_name} cannot be found. Please check your Python installation.")
	
	if "PYTHONHOME" in os.environ:
		python_home_val = os.path.expandvars(os.environ['PYTHONHOME'])
		python311_path = os.path.dirname(dll_path)
		if os.path.samefile(python_home_val, python311_path):
			# PYTHONHOME is set
			return
		else:
			raise Exception(f"PYTHONHOME exists and set to {python_home_val}, while {python311_path} is expected.")
	
	# set PYTHONHOME
	dll_dir = os.path.dirname(dll_path)
	
	reply = ask_user(f"PYTHONHOME environment variable is not set. Do you want me to set it to {dll_dir}?", 'y', ['y', 'n'])
	reply = reply.strip()
	if reply.lower() == '' or reply.lower() == 'y':
		print('Setting PYTHONHOME')
		set_windows_user_environment_variable('PYTHONHOME', dll_dir)
	else:
		raise Exception(f'Set PYTHONHOME environment variable to {dll_dir} and try again')


def refresh_windows_env():
	import winreg
	
	# system environment variables
	key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, "System\\CurrentControlSet\\Control\\Session Manager\\Environment", access=winreg.KEY_READ)
	
	# get the number of values in the key
	num_values = winreg.QueryInfoKey(key)[1]
	
	# loop through the values
	for i in range(num_values):
		# get the name and value of the environment variable
		name, value, _ = winreg.EnumValue(key, i)
		# update the os.environ dictionary with the new value
		value = os.path.expandvars(os.path.expanduser(value))
		os.environ[name] = value
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))
	
	# user environment variables
	# open the registry key for the current user's environment variables
	key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Environment", access=winreg.KEY_READ)
	
	# get the number of values in the key
	num_values = winreg.QueryInfoKey(key)[1]
	
	# loop through the values
	for i in range(num_values):
		# get the name and value of the environment variable
		name, value, _ = winreg.EnumValue(key, i)
		# update the os.environ dictionary with the new value
		value = os.path.expandvars(os.path.expanduser(value))
		if name in os.environ and name.lower() == 'path':
			os.environ[name] += ';' + value
		else:
			os.environ[name] = value
	
	# close the registry key
	winreg.CloseKey(key)
	
	# broadcast a message to all windows that the environment has changed
	HWND_BROADCAST = 0xFFFF
	WM_SETTINGCHANGE = 0x1A
	SMTO_ABORTIFHUNG = 0x0002
	result = ctypes.c_long()
	SendMessageTimeoutW = ctypes.windll.user32.SendMessageTimeoutW
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, u"Environment", SMTO_ABORTIFHUNG, 5000, ctypes.byref(result))


def refresh_ubuntu_env():
	from dotenv import load_dotenv
	
	load_dotenv("/etc/environment")
	load_dotenv("~/.profile")
	

if is_windows():
	refresh_env = refresh_windows_env
elif is_ubuntu():
	refresh_env = refresh_ubuntu_env


def check_windows_java_jni_installed(version: str):
	java_path = shutil.which("java.exe")
	if java_path is None:
		is_install_java = ask_user("Java was not detected, do you want me to install it for you?", 'y', ['y', 'n'])
		if is_install_java == 'y':
			install_windows_openjdk(version)
			refresh_windows_env()
		else:
			raise Exception('JAVA_HOME is not set. Make sure JVM is installed and JAVA_HOME environment variable is set and try again.')
	
	# try to get the value of JAVA_HOME environment variable
	java_home = os.environ.get("JAVA_HOME")
	java_home = os.path.expanduser(os.path.expandvars(java_home))
	
	if not java_home:
		raise Exception('Java was found, but JAVA_HOME is not set, check your JVM installation.')
	
	# construct the path to jvm.dll
	jvm_path = os.path.join(java_home, "bin", "server", "jvm.dll")
	
	if not os.path.exists(jvm_path):
		raise Exception(f'Cannot find jvm.dll for JNI at {jvm_path}. Please check your installation to include JNI and try again.')
	
	# try to load jvm.dll
	add_to_path_environment_variable(os.path.dirname(java_home))
	add_to_path_environment_variable(os.path.dirname(java_home + '\\bin\\server\\'))
	refresh_env()
	
	try:
		dll = ctypes.cdll.LoadLibrary(jvm_path)
	except OSError as exp:
		raise Exception(f"Failed to load {jvm_path}. {exp.strerror}. Please check {os.path.dirname(jvm_path)} is in PATH environment variable and try again.")
	
	# Check it is version 11
	command = "java -version"
	# run the command and capture the output
	error_code, stdout, stderr = run_command(command)
	
	# check if the output contains the word JNI
	if f'version "{version}' not in stdout and f'version "{version}' not in stderr:
		raise Exception(f'MetaFFI currently supports JVM Version {version} is supported, while the installed JVM is:\n{stderr}\nInstall JVM version {version} and try again.')


def check_windows_prerequisites():
	print('checking prerequisites...')
	
	# python
	check_python_windows_installed('3.11')
	check_windows_pythonhome('3.11')
	
	# openjdk
	check_windows_java_jni_installed('21')
	
	# go
	check_go_installed(install_windows_go)
	#patch_windows_go()
	
	# gcc
	install_windows_gcc()


def install_windows():
	global windows_x64_zip
	
	# verify running as admin
	is_admin = ctypes.windll.shell32.IsUserAnAdmin() != 0
	if not is_admin:
		raise Exception('User must have admin privileges')
	
	refresh_windows_env()  # refresh environment variables, in case the environment is not up-to-date
	
	check_windows_prerequisites()
	
	print()
	print('==== Starting installation ====')
	print()
	
	# get install dir
	install_dir = get_install_dir("c:\\MetaFFI\\")
	
	# unpack zip into install dir
	unpack_into_directory(windows_x64_zip, install_dir)
	
	# setting METAFFI_HOME environment variable
	set_windows_system_environment_variable("METAFFI_HOME", install_dir)
	
	# add install_dir and install_dir\bin to PATH
	add_to_path_environment_variable(install_dir)
	add_to_path_environment_variable(install_dir + '\\bin\\')
	
	add_metaffi_home_to_cgo_cflags(install_dir, set_windows_system_environment_variable)
	
	install_apis()
	
	print()
	print('==== Running Sanity Tests ====')
	print()
	
	run_sanity_tests()
	
	print('Done')


# -------------------------------


def get_ubuntu_environment_variable(file: str, name: str) -> str | None:
	# Execute the command and capture the output
	err_code, stdout, stderr = run_shell(f"grep -q '{name}=' {file}")
	
	if err_code != 0:
		return None
	
	err_code, stdout, stderr = run_shell(f"grep '{name}=' {file} | cut -d '=' -f 2")
	
	# Check the return code and get the value
	if err_code != 0:
		raise Exception(f'Failed to get the value of the environment variable in {file}: {stdout}{stderr}')
	
	return stdout.strip()


def get_ubuntu_user_environment_variable(name: str) -> str | None:
	return get_ubuntu_environment_variable(name, '~/.profile')


def get_ubuntu_machine_environment_variable(name: str) -> str | None:
	return get_ubuntu_environment_variable(name, '/etc/environment')


def set_ubuntu_environment_variable(file: str, name: str, value: str):
	existing_val = get_ubuntu_environment_variable(name, file)
	
	if existing_val is not None:  # env var exists
		if existing_val == value:
			return
		
		update_value = ask_user(f'Existing {name} is {existing_val}, do you want me to update it to {value} in {file}?', 'y', ['y', 'n'])
		if update_value == 'n':
			raise Exception(f'{name} must be {value} in order to continue. Make sure {name} points to the required python and try again')
		
		print(f'Updating environment variable {name}={value} in {file}')
		
		# update value
		err_code, stdout, stderr = run_shell(f"sed -i 's/{name}={existing_val}/{name}={value}/g' {file}")
		if err_code != 0:
			raise Exception(f'Failed to update {name} environment variable in {file}: {stdout}{stderr}')
		return
	
	else:  # var doesn't exist
		print(f'Adding environment variable {name}={value} in {file}')
		err_code, stdout, stderr = run_shell(f"echo 'export {name}={value}' >> {file}")
		if err_code != 0:
			raise Exception(f"Failed to add {name} environment variable to {file}: {stdout}{stderr}")
	
	refresh_env()


def set_ubuntu_user_environment_variable(name: str, value: str):
	set_ubuntu_environment_variable('~/.profile', name, value)


def set_ubuntu_machine_environment_variable(name: str, value: str):
	set_ubuntu_system_environment_variable(name, value)


def check_python_ubuntu_installed(version: str):
	# split the version number into major, minor, and micro parts
	major, minor = map(int, version.split("."))
	
	# construct the executable file name based on the version number
	exe_name = f"python{major}.{minor}"
	
	# try to run the executable file using subprocess
	exit_code, stdout, stderr = run_command(f'{exe_name} --version')
	
	if exit_code != 0 or not stdout.strip().startswith(f"Python {version}"):
		reply = ask_user(f'Python {exe_name} is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f"{exe_name} cannot be found. Please check your Python {version} is installed, and in PATH environment variable")
		
		# install python
		exit_code, stdout, stderr = run_command(f'apt install {exe_name} -y')
		if exit_code != 0:
			raise Exception(f'Failed to install {exe_name}. Output\n{stdout}{stderr}')


def check_ubuntu_pythonhome(version: str):
	major, minor = map(int, version.split("."))
	
	# construct the executable file name based on the Python version
	exe_name = f"python{major}.{minor}"
	
	# try to get the full path of the executable file using os
	try:
		exe_path = shutil.which(exe_name)
	except OSError:
		raise Exception(f"{exe_name} cannot be found. Please check your Python installation.")
	
	if "PYTHONHOME" in os.environ:
		python_home_val = os.path.expanduser(os.environ['PYTHONHOME'])
		maybe_python311_path = os.path.dirname(exe_path)
		if os.path.samefile(python_home_val, maybe_python311_path):
			# PYTHONHOME is set
			return
		else:
			raise Exception(f"PYTHONHOME exists, but set to {python_home_val}, while {exe_path} is expected.")
	
	# set PYTHONHOME
	exe_dir = os.path.dirname(exe_path)
	
	reply = ask_user(f"PYTHONHOME environment variable is not set. Do you want me to set it to {exe_dir}?", 'y', ['y', 'n'])
	if reply.lower() == 'y':
		print('Setting PYTHONHOME')
		set_ubuntu_user_environment_variable('PYTHONHOME', exe_dir)
	else:
		raise Exception(f'Set PYTHONHOME environment variable to {exe_dir} and try again')


def get_java_home(version: str) -> str | None:
	output = os.popen("update-java-alternatives -l").read()
	lines = output.split("\n")
	
	for line in lines:
		# use regular expression to extract the name, priority and path of the alternative
		match = re.search(r"(\S+)\s+(\d+)\s+(\S+)", line)
		if not match:
			continue
		
		# get the name of the alternative
		name = match.group(1)
		
		# check if the name contains the given version
		if version in name:
			path = match.group(3)
			return path
	
	return None


def check_ubuntu_java_jni_installed(version: str):
	# Execute the command and get the output
	err_code, stdout, stderr = run_command(f"java -version {version}")
	
	# Check the return code and print the result
	if err_code != 0:
		
		reply = ask_user(f'JVM {version} is not installed. Do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'JVM {version} must be installed in order to continue the installation. Install JVM {version} and try again.')
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(f"apt install -y openjdk-{version}-jdk")
		refresh_env()
		
		if err_code != 0:
			raise Exception(f"Failed to install openjdk-{version}-jdk with error code {err_code}. Output:\n{stdout}{stderr}")
	
	java_location = get_java_home(version)
	
	if "JAVA_HOME" in os.environ and not os.environ["JAVA_HOME"] == java_location:
		cur_java_home = os.environ["JAVA_HOME"]
		raise Exception(f"JAVA_HOME is already set, but it is set to {cur_java_home} and not to {java_location}. Update the environment variable and try again")
	
	# set JAVA_HOME
	set_ubuntu_machine_environment_variable('JAVA_HOME', java_location)
	
	# Try to load libjvm.so
	loaded_jni = False
	try:
		ctypes.cdll.LoadLibrary('libjvm.so')
		loaded_jni = True
	except:
		pass
	
	if not loaded_jni:
		
		# make sure libjvm.so exists
		jni_path = f'{java_location}/lib/server/libjvm.so'
		if not os.path.exists(jni_path):
			raise Exception(f'{jni_path} not found. Please check your JVM installation and try again.')
		
		reply = ask_user(f'failed to load {jni_path}. Do you want me to fix it by placing a symbolic link of {jni_path} to /usr/lib/?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception(f'Please make sure libjvm.so can be loaded, and try again')
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(f"ln -s {jni_path} /usr/lib/libjvm.so")
		if err_code != 0:
			raise Exception(f'Failed to create a symbolic link from {jni_path} to /usr/lib/libjvm.so. Output:\n{stdout}{stderr}')
		
		try:
			ctypes.cdll.LoadLibrary('libjvm.so')
		except Exception as e:
			raise Exception(f'Although libjvm.so has been linked to /usr/lib/, I still cannot load it.\nError: {str(e)}')


def verify_pip_packages_installed():
	install_pip_package('python-dotenv')


def install_ubuntu_go():
	import requests
	
	# Define the command to check if Go exists
	command = "which go"
	
	# Execute the command and get the output
	err_code, stdout, stderr = run_command(command)
	
	# Check the return code and print the result
	if err_code == 0:
		
		# make sure go has supported version
		
		# Define the command to check the go version
		command = "go version"
		
		# Execute the command and get the output
		err_code, stdout, stderr = run_command(command)
		
		# Check the return code and get the value
		if err_code != 0:
			raise Exception(f"Although Go is installed, failed the command {command} with error code {err_code}. Output:\n{stdout}{stderr}")
		
		go_version = stdout.split()[2].replace("go", "")
		
		# Compare the go version with the minimum required version
		if go_version < "1.21.0":
			# The go version is below 1.21.0
			# Raise an exception with the error message
			raise Exception(f"Go version {go_version} is not supported. Please install at least Go 1.21.0 and try again.")
		
		# make sure CGO_ENABLED=1
		err_code, stdout, stderr = run_command("go env -w CGO_ENABLED=1")
		if err_code != 0:
			raise Exception(f"Failed settings CGO_ENABLED=1. error code {err_code}. Output:\n{stdout}{stderr}")
	
	else:  # Go does not exist
		
		# install go 1.21.5
		dl_link = f"https://go.dev/dl/go1.22.2.linux-amd64.tar.gz"
		
		with tempfile.TemporaryDirectory() as tempdir:
			# Get the file name from the link
			filename = dl_link.split("/")[-1]
			filepath = os.path.join(tempdir, filename)
			
			urllib.request.urlretrieve(dl_link, filepath)
			
			# Extract the file to "/usr/local"
			command = f"tar -C /usr/local -xzf {filepath}"
			
			# Execute the command and get the output
			err_code, stdout, stderr = run_command(command)
			
			# Check the return code and print the result
			if err_code != 0:
				raise Exception(f"Failed extracting downloaded Go using the command {command}. Failed with error code {err_code}. Output:\n{stdout}{stderr}")
		
		run_command(f'ln -s /usr/local/go/bin/go /usr/bin/go', True)
		
		# Update the PATH environment variable to include /usr/local/go/bin
		command = f"echo 'export PATH=$PATH:/usr/local/go/bin' | tee -a /etc/environment"
		err_code, stdout, stderr = run_command(command)
		if err_code != 0:
			raise Exception(f"Failed to add Go to PATH using the command {command}, which failed with error code {err_code}. Output:\n{stdout}{stderr}")
		
		# set environment variable CGO_ENABLED=1
		set_ubuntu_system_environment_variable('CGO_ENABLED', '1')


def check_ubuntu_gcc_installed():
	err_code, stdout, stderr = run_command("gcc --version")
	if err_code != 0:
		
		reply = ask_user('GCC is not installed, do you want me to install it for you?', 'y', ['y', 'n'])
		if reply == 'n':
			raise Exception("Please install GCC and try again.")
		
		command = "apt install gcc -y"
		err_code, stdout, stderr = run_command(command)
		
		if err_code != 0:
			raise Exception(f"Failed to install GCC using the {command}. error code {err_code}. Output:\n{stdout}{stderr}")


def check_pip_installed():
	exit_code, stdout, stderr = run_command('add-apt-repository ppa:deadsnakes/ppa -y', False, False)
	if exit_code != 0:
		raise Exception(f'Failed to add ppa:deadsnakes/paa. Output\n{stdout}{stderr}')
	
	err_code, stdout, stderr = run_command(f'{sys.executable} -m pip --version', False, False)
	if err_code != 0:
		err_code, stdout, stderr = run_command(f'apt install python3-pip -y', False, False)
		if err_code != 0:
			raise Exception(f'Failed to install pip with error code: {err_code}. Output:\n{stdout}{stderr}')


def check_ubuntu_prerequisites():
	print('checking prerequisites...')
	
	check_pip_installed()
	verify_pip_packages_installed()
	
	refresh_env()  # refresh environment variables, in case the environment is not up-to-date
	
	# python
	check_python_ubuntu_installed('3.11')
	check_ubuntu_pythonhome('3.11')
	
	# openjdk
	check_ubuntu_java_jni_installed('21')
	
	# go
	check_go_installed(install_ubuntu_go)
	
	# gcc
	check_ubuntu_gcc_installed()


def set_ubuntu_system_environment_variable(name: str, value: str):
	# construct the file name for the environment file
	env_file = "/etc/environment"
	
	# read the existing lines from the environment file
	with open(env_file, "r") as f:
		lines = f.readlines()
	
	# check if the environment variable already exists in the environment file
	for i, line in enumerate(lines):
		if line.startswith(f"{name}="):
			# get the current value of the environment variable
			current_value = line.split("=")[1].strip()
			if current_value == value:
				# if the value is the same, do nothing
				return
			else:
				# if the value is different, replace the line with the new value
				lines[i] = f"{name}={value}\n"
				break
	else:
		# if the environment variable does not exist, append a new line with the name and value
		lines.append(f"{name}={value}\n")
	
	# write the modified lines back to the environment file
	with open(env_file, "w") as f:
		f.writelines(lines)
	
	refresh_env()


def make_metaffi_available_globally(install_dir: str):
	run_command(f'chmod u+x {install_dir}/metaffi', True)
	run_command(f'ln -s {install_dir}/metaffi /usr/bin/metaffi', True)


def install_ubuntu():
	global ubuntu_x64_zip
	
	# verify running as admin
	is_admin = os.getuid() == 0
	if not is_admin:
		raise Exception('Installer must run as sudo')
	
	check_ubuntu_prerequisites()
	
	print()
	print('==== Starting installation ====')
	print()
	
	# get install dir
	install_dir = get_install_dir("/usr/local/metaffi/")
	
	# unpack zip into install dir
	unpack_into_directory(ubuntu_x64_zip, install_dir)
	
	make_metaffi_available_globally(install_dir)
	
	# setting METAFFI_HOME environment variable
	set_ubuntu_system_environment_variable("METAFFI_HOME", install_dir)
	
	add_metaffi_home_to_cgo_cflags(install_dir, set_ubuntu_machine_environment_variable)
	
	refresh_env()
	install_apis()
	
	if 'METAFFI_HOME' not in os.environ:
		raise Exception('METAFFI_HOME not in os.environ')
	
	print()
	print('==== Running Sanity Tests ====')
	print()
	
	global is_skip_tests
	global is_extended_tests
	
	if not is_skip_tests:
		run_sanity_tests()
		
		if is_extended_tests:
			run_extended_tests()
	
	print('Done')


# -------------------------------


def set_installer_flags():
	global is_silent
	global is_skip_tests
	global is_extended_tests
	global is_run_windows_patched_go_tests
	
	for arg in sys.argv:
		arg = arg.lower()
		
		if arg == '-h' or arg == '--help' or arg == '/?' or arg == '/h':
			print('MetaFFI Installer - Installs MetaFFI and Python3.11, Go and OpenJDK plugins')
			print('-s - silent mode (using defaults)')
			print('--skip-sanity - skips all tests after installation')
			print('--include-extended-tests - runs extended tests after installation (downloads several 3rd party libraries for tests)')
			print('--patched-go (windows only) - Assume Go is patched and able to run Go -> OpenJDK tests (https://github.com/golang/go/issues/58542)')
			return False
		
		if arg == "/s" or arg == "-s":
			is_silent = True
		
		if arg == '--skip-sanity':
			is_skip_tests = True
		
		if arg == '--include-extended-tests':
			is_extended_tests = True
			
		if arg == '--patched-go':
			is_run_windows_patched_go_tests = True
			
	return True


def main():
	if not set_installer_flags():  # returns is continue running installer
		return
	
	try:
		if platform.system() == 'Windows':
			install_windows()
		elif platform.system() == 'Linux':
			import distro
			if distro.name() == 'Ubuntu':
				install_ubuntu()
			else:
				print("Installer supports Ubuntu distribution, not {}".format(distro.name()), file=sys.stderr)
				exit(1)
		else:
			print("Installer doesn't support platform {}".format(platform.system()), file=sys.stderr)
			exit(1)
	except Exception as exp:
		traceback.print_exc()
		exit(2)
	
	print('\nInstallation Complete!\nNotice you might need to logout/login or reboot to apply environmental changes\n')


if __name__ == '__main__':
	main()
