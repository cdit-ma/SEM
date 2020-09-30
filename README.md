# MEDEA - A graphical modelling and analysis environment for Systems of Systems

MEDEA (Modelling, Experimentation, DEsign and Analysis) is a graphical modelling environment
created to assist with Model Driven Engineering.
It features domain-specific semantics for fine and coarse grained modelling of large
distributed software systems of systems, as well as a RE (Runtime Environment) backend that
provides native deployment and execution capabilities.

For more details regarding this project's aims, capabilities and outcomes see the
[project website](https://cdit-ma.github.io/).

This work has been developed with the support of the Australian Government Defence Science and Technology Group.

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

The following dependencies are required in order to generate the base project

##### Linux platforms:
The scripts for automating dependencies can be found in the [re/scripts](re/scripts) folder.

Dependencies to be installed manually:
* Qt 5.14 (Optional, required for building the GUI)

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

##### Windows:
The following dependencies should be installed:
* Microsoft Visual Studio 2019
* Java Runtime Environment 8
* Python 3
* Boost 1.67
* NASM
* PostgreSQL 12
* Docker Desktop (Optional)
* Ninja (Optional)
* Qt 5.14 (Optional, required for building the GUI)

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

By default, CMake will attempt to detect the most appropriate build system for your platform,
although this can be overridden. For example, if the Ninja build system is installed, build
times may be significantly lowered with the following alternative generation command:

```
cmake -G Ninja ..
```

The project binaries can then be built by providing the build flag to cmake and specifying
the root of the build directory:

```
cmake --build .
```
