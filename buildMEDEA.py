import os, zipfile, subprocess, platform, sys
WIN_SPEC = "win32-msvc2010"
LINUX_SPEC = "linux-g++"
MAC_SPEC = "macx-clang"

QT_VERSION = "5.5"
MAC_VERSION = "clang_64"
WIN_VERSION = "msvs2010"
LINUX_VERSION = "linux-g++"

def print_title(message):
	print("\t* " + message)

def clean_built_dir(dir):
	print_title("Cleaning directory")
	remove_extensions=[".obj",  ".cpp", ".o", ".manifest", ".res", "state.ini"]
	old_dir = os.getcwd()
	os.chdir(dir)
	for dirname, subdirs, files in os.walk("."):
		for filename in files:
			fullname = os.path.join(dirname, filename)
			for remove_extension in remove_extensions:
				if  filename.endswith(remove_extension):
					os.remove(fullname)
	os.chdir(old_dir)
	
def zip_dir(zip_file_name, zip_directory):
	print_title("ZIPing executables")
	# Store the old directory
	old_dir = os.getcwd()
	# Change to the directory we wish to swap to
	os.chdir(zip_directory)
	# Open a zip file with compression.
	zf = zipfile.ZipFile(zip_file_name, "w", zipfile.ZIP_DEFLATED)
	for dirname, subdirs, files in os.walk("."):
		# Write the directory
		zf.write(dirname)
		# Write the files.
		for filename in files:
			zf.write(os.path.join(dirname, filename))
	zf.close()
	# Change directory back.
	os.chdir(old_dir)

def is_unix():
	return "linux" in sys.platform

def is_windows():
	return "win" in sys.platform

def is_macosx():
	return "darwin" in sys.platform

def get_qmake_spec():
	if is_windows():
		return WIN_SPEC
	elif is_macosx():
		return MAC_SPEC
	else:
		return LINUX_SPEC

def run_qmake(dir):
	qmake_spec =  "win32-msvc2010"
	#qmake MEDEA.pro -r -spec win32-msvc2010 linux-g++ macx-clange (CONFIG+=x86_64)
	
def run_qtdeploy(dir, name):
	global QT_BIN
	if is_unix():
		return 0
	
	command = "windeployqt " + name + ".exe"
	if is_macosx():
		command = "macdeployqt " + name + ".app"
	
	command += " --no-translations"
	print_title("Running QT Deploy")
	
	results = run_command(command, dir)
	
	if results[0]  != 0:
		print("\tFailure!")
		for line in results[2] :
			print(line)
	return results[0]


def run_command(command, dir):
	process = subprocess.Popen(command, shell=True, stdout = subprocess.PIPE, stderr = subprocess.PIPE, cwd = dir)
	 # Communicate will wait for the process to finish.
	stdout, stderr = process.communicate()
	return_code = process.returncode
	stdout_lines = []
	stderr_lines = []
	for line in stdout.decode("utf-8").split("\n"):
		stdout_lines.append(line)
	for line in stderr.decode("utf-8").split("\n"):
		stderr_lines.append(line)
	return [return_code, stdout_lines, stderr_lines]

def run_make(dir):
	global QT_DIR
	command = ""
	if  is_windows():
		# Get into Visual Studio mode.
		win_vc = "\"C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC/vcvarsall.bat\" x86"
		command += win_vc + " & "
	
	if is_windows():
		# Windows Uses Jom in QT Creator for compilation
		JOM_ROOT = "\"" + QT_DIR + "/Tools/QtCreator/bin/jom.exe\" install"
		command += JOM_ROOT
	else:
		command += "make install"
	
	print_title("Compiling MEDEA")
	results = run_command(command, dir)
	if results[0] != 0:
		print("\tFailure!")
		for line in results[2] :
			print(line)
			
	return results[0]

QT_DIR = "C:/Qt/Qt5.5.0/"
if is_windows():
	QT_BIN = QT_DIR + "/" + QT_VERSION+ "/" + WIN_VERSION + "/bin/"
elif is_mac():
	QT_BIN = QT_DIR + "/" + QT_VERSION+ "/" + MAC_VERSION + "/bin/"
else:
	QT_BIN = QT_DIR + "/" + QT_VERSION+ "/" + LINUX_VERSION + "/bin/"

build_dir = "C:/Test/MEDEA/"
run_make(build_dir)
run_qmake(build_dir)
output_dir = build_dir + "/release/"
run_qtdeploy(output_dir, "MEDEA")
clean_built_dir(output_dir)
zip_dir(build_dir+ "MEDEA.zip",  output_dir)







