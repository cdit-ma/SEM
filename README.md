# MEDEA - A graphical modelling and analysis environment for Systems of Systems

MEDEA (Modelling, Experiment DEsign and Analysis) is a graphical modelling tool
created to assist with System Execution Modelling.
It features domain-specific semantics for fine and coarse grained modelling of large
distributed software systems of systems, as well as a RE (Runtime Environment) backend that
provides native deployment and execution capabilities.

For more information regarding this project's aims, capabilities and outcomes see the
[project website](https://cdit-ma.github.io/).

This work has been developed by the Modelling and Analysis Group within the School of
Computer Science at The University of Adelaide.

This work has been developed with the support of the Australian Government Defence
Science and Technology Group.

## Table of Contents

* [Supported Platforms](#supported-platforms)
* [Build Instructions](#build-instructions)

## Supported Platforms
Pull requests are built and tested on the following platforms:
* Ubuntu 18
* Centos 7
* macOS Catalina
* Windows 10

## Build Instructions

### Dependencies

There are many dependencies required for a complete installation of the SEM project, falling
into the following categories.

If you notice anything missing please create an issue that describes both the error received
 as well as the platform on which you encountered it.

##### Linux platforms:
It is recommended that you follow the steps outlined in the automated installation scripts,
which can be found in the [re/scripts](re/scripts) folder.

Dependencies to be installed manually:
* Qt 5.14 (Optional, required for building the GUI)
* Middlewares
  * RTI DDS Connext 5.3.0
  * OpenSplice DDS 6.4.14
  * Apache Qpid Proton 0.24.0
  * Apache Qpid C++ 1.38.0
  * ACE TAO 6.5.0

##### macOS:
Suggested dependency installation script:
```
brew install coreutils
brew install cmake
brew install curl
brew install boost
brew install libpq
brew install postgresql # optional
brew install openjdk
```
Other dependencies to be installed:
* Qt 5.14 (Optional, required for building the GUI)

* Middlewares
  * RTI DDS Connext 5.3.0
  * OpenSplice DDS 6.4.14
  * ACE TAO 6.5.0

##### Windows:
The following dependencies should be installed:
* Microsoft Visual Studio 2019
* Java Runtime Environment 8
* Python 3
* Boost 1.67
* NASM
* PostgreSQL 12
* Qt 5.14 (Optional, required for building the GUI)
* Docker Desktop (Optional)
* Ninja (Optional)
* Doxygen (Optional)

* Middlewares
  * RTI DDS Connext 5.3.0
  * OpenSplice DDS 6.4.14
  * Apache Qpid Proton 0.24.0
  * Apache Qpid C++ 1.38.0
  * ACE TAO 6.5.0

### Build From Source

##### Quick Build Instructions
```
git clone https://github.com/cdit-ma/SEM
cd sem
mkdir build
cd build
cmake -G ..
cmake --build .
```

##### Build Instruction Breakdown

From within the cloned project directory, create a new build directory to use for the project.

```
mkdir build
cd build
```

CMake's cross-platform build generation command can then be used to generate specific build
system files in the current directory. The directory containing the projects source code
(specifically the CMakeLists.txt file) must be specified.

During generation CMake will attempt to pull down and generate the project source code for
several third-party projects upon which SEM depends. The details for these third party
projects can be found in the [extern](extern) folder.

```
cmake -G ..
```

The project binaries can then be built by providing the build flag to cmake and specifying
the root of the build directory:

```
cmake --build .
```

##### CMake Generation Customisation

By default, CMake will attempt to detect the most appropriate build system for your platform,
although this can be overridden. For example, if the Ninja build system is installed, build
times may be significantly lowered with the following alternative generation command:

```
cmake -G Ninja ..
```

There are also several flags available for opting into or out of building various parts
of the project. The format for setting an option named OPTION to have a given value makes
use of the -D flag and takes the form ```cmake -G .. -DOPTION=Value```.

The following list provides some of the exposed options:
* BUILD_MEDEA: (default = ON) Enables/disables the building of the graphical user interface,
 not required for backend nodes
* BUILD_TEST: (default = OFF) Enables/disables the building of tests
* BUILD_DOCS: (default = OFF) Enables/disables the building of doxygen documentation
* NO_SIGAR: (default = ON) Enables/disables the building of the SIGAR based logging framework,
 may not be compatible with some newer platforms.
* SEM_DEPS_BASE: The filepath to a directory that will contain any externally fetched
 dependencies to be downloaded. Can be used to assist with dependency caching, with
 application in CI and air-gapped builds.
