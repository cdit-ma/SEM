# logan
logan is a lightweight, high performance, cross-platform, distributed hardware logging system. It is designed to aggregate system hardware performance metrics to facilitate analysis.

### logan server
logan server connects to one or many logan clients and stores all received metrics in a SQLite3 database.

### logan client
logan client utilises the sigar system monitoring interface to collect system metrics and ZeroMQ to communicate with one or many logan servers. Logging can be run in live or cached mode to optimise for either network traffic or disk I/O. Logging frequency can also be specified to control data point granularity.

## Supported Operating Systems:
* Windows 7 (> Visual Studio 2015)
* MacOSX 10.10 (Or Newer)
* Ubuntu 14.04 (or Newer)

## Required Libraries:
* https://github.com/google/protobuf
* https://github.com/zeromq/libzmq
* https://github.com/hyperic/sigar

## Build Tool:
* https://github.com/Kitware/CMake

## Installation:
* Build and install all required libraries
* ``git clone --recursive https://github.com/cdit-ma/logan``
* ``cd logan``
* ``mkdir build``
* ``cd build``
* ``cmake .. -DBUILD_SERVER=ON -DBUILD_CLIENT=ON``
* To disable re-model or hardware logging, ``-DDISABLE_MODEL_LOGGING`` or ``-DDISABLE_HARDWARE_LOGGING`` can be added to above cmake command.
* **Unix -** ``make``
* **Windows -** ``msbuild logan.sln /p:Configuration=Release``
* Executables will be placed in logan/bin
