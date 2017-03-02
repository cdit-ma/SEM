# logan
logan is a lightweight, high performance, cross-platform, distributed hardware logging system. It is designed to aggregate system hardware performance metrics to enable analysis.

##Supported Operating Systems:
* Windows 7 (> Visual Studio 2015)
* MacOSX 10.10 (Or Newer)
* Ubuntu 14.04 (or Newer)

##Required Libraries:
* https://github.com/google/protobuf
* https://github.com/zeromq/libzmq
* https://github.com/hyperic/sigar
* https://github.com/cdit-ma/re_common
* https://sqlite.org/

##Build Tools:
* https://github.com/Kitware/CMake

##Installation:
* Build and install all required libraries
* ``cmake . -DBUILD_SERVER=ON -DBUILD_CLIENT=ON``
* **Unix -** ``make``
* **Windows -** ``msbuild logan.sln /p:Configuration=Release``





