# Installation
1. Ensure required libraries are installed
2. Modify re_configure.sh to point at correct library root dirs
3. ``./re_configure.sh``
4. ``git clone --recursive https://github.com/cdit-ma/re``
5. ``cd re``
6. ``mkdir build``
7. ``cd build``
8. ``cmake .. -G Ninja``
9. * **Unix -** ``cmake --build .``
   * **Windows -** ``msbuild logan.sln /p:Configuration=Release``

Executables will be placed in re/bin

# Executables
## cpu_profiler
Simple cpu profiler. Calculates number of MWIPs operations executable in one second.
## re\_node\_manager
Core node manager. Controls registration of nodes, construction/destruction of components, activation of components and teardown of node.
## re\_node\_parser
Used to pull deployment information out of Model GraphML file.
## Tests
### cpu\_worker\_tester
### gpu\_worker\_tester
### memory\_worker\_tester
### utility\_worker\_tester