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

## Usage
A command line version of MEDEA, provides command line import/export functionality. Returns an error code if failed.

### Server command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
<<<<<<< HEAD
| -c, --clients [arg list]              | List of logan_client endpoints to connect to|
| -d, --database [arg]                  | Filename of output database  |
=======
| -c, --clients [List of logan client endpoints] | Displays version information.|
| -d, --database [File name of output database]  | Open a graphml project.      |
>>>>>>> 85685e716eb19188a0a40b7063bef4ffc7b2024b

### Client command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
| -s, --system_info_print               | Print system info then exit           |
| -l, --live_mode [arg (=0)]            | Produce data live                     |
<<<<<<< HEAD
| -P, --process [arg list]              | Monitor specific processes            |
=======
| -P, --process [List of process names] | Monitor specific processes            |
>>>>>>> 85685e716eb19188a0a40b7063bef4ffc7b2024b
| -f, --frequency [arg (=1)]            | Logging frequency in Hz               |
| -p, --publisher [arg]                 | Publisher endpoint (ie tcp://192.168.111.1:5555)|


### Example Usage
<<<<<<< HEAD
Running a simple logging scenario with one logging server and two logging clients.
=======
Running a simple logging scenario with one logging server and one logging client.
>>>>>>> 85685e716eb19188a0a40b7063bef4ffc7b2024b
```
>Start client 1 (Logs every 2 seconds, with extra information about a process called "logan_server")
./logan_client -p tcp://192.168.111.100:5555 -f 0.5 -p logan_server
>Start client 2 (Logs every 2 secondts, sending information as it is generated)
./logan_client -p tcp://192.168.111.100:5556 -f 0.5 --live_mode true
>Start server listening to both clients and outputting to "output.sql"
./logan_server -c tcp://192.168.111.100:5555 tcp://192.168.111.100:5556 -d output.sql
```