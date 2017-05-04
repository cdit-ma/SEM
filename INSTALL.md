# Installation
* Ensure required libraries are installed
* Modify re_configure.sh to point at correct library root dirs
* ``git clone --recursive https://github.com/cdit-ma/re``
* ``cd re``
* ``mkdir build``
* ``cd build``
* ``cmake ..``
* **Unix -** ``make``
* **Windows -** ``msbuild logan.sln /p:Configuration=Release``
* Executables will be placed in logan/bin
