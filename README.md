# MEDEA
## About MEDEA (Modelling, Experiment DEsign and Analysis)
MEDEA is a tool that can be used to design and analyse performance of Distributed Real-time and Embedded (DRE) systems. The tool is designed to facilitate the creation of a representative model of a distributed system, as well as providing System Execution Modelling (SEM) and performance analysis.

## Minimum software requirements:
* [CMake 3.0](https://cmake.org/)
* [Qt 5.7.0](https://www.qt.io/)
* C++11 Compiler

## Optional software requirements:
* [Qt Installer Framework 2.0](http://doc.qt.io/qtinstallerframework/)
* [Ninja](https://ninja-build.org/)


# Building MEDEA binary from source
* Set environment variable Qt5_DIR to point the installation of QT5.7 (ie /opt/Qt5.7.0/5.7/gcc_64)
* Replace the -G flag with a suitable generator for your operating system, "Unix Makefiles" for Unix
> ```
> git clone https://github.com/cdit-ma/MEDEA --recursive
> cd MEDEA
> mkdir build
> cd build
> cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
> cmake --build . --config Release
> ```

# Building MEDEA installer from source
* Set environment variable QTIFWDIR to point the installation of QT Installer Framework
* Follow the build instructions above
> ```
> cpack -G IFW .
> ```
* The installer will be located in build/_CPack_Packages