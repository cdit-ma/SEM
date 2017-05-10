# MEDEA
## About MEDEA (Modelling, Experiment DEsign and Analysis)
MEDEA is a tool that can be used to design and analyse performance of Distributed Real-time and Embedded (DRE) systems. The tool is designed to facilitate the creation of a representative model of a distributed system, as well as providing System Execution Modelling (SEM) and performance analysis.

## Supported Operating Systems:
* Windows 7 (Or Newer)
* MacOSX 10.10 (Or Newer)
* Ubuntu 14.04 (or Newer)

## Minimum Software Requirements:
* [Qt 5.7.0](https://www.qt.io/)
* [CMake 3.0](https://cmake.org/)
* [Oracle Java 7 Runtime](http://www.oracle.com/technetwork/java/javase/downloads/index.html)

# Building MEDEA binary from source
* Set environment variable Qt5_DIR to point the installation of QT5.7 (Qt5_DIR=/Users/dan/Qt5.7.0/5.7/clang_64/)
> ```
> git clone https://github.com/cdit-ma/MEDEA --recursive
> cd MEDEA
> mkdir build
> cd build
> cmake ..
> make -j10
> ```

## Building MEDEA installer from source
1. Set environment variable Qt5_DIR to point the installation of QT5.7
2. Set environment variable QTIFWDIR to point the installation of QT Installer Framework
3. Follow the build instructions above
4. `make package`
* The installer will be located in build/_CPack_Packages