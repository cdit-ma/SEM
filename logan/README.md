# logan
logan is a lightweight, high performance, cross-platform, distributed hardware logging system. It is designed to aggregate system hardware performance metrics to facilitate analysis.

Both executables are shell tools, which will run indefinately until a `Ctrl+C` or `SIGINT` interupts the process, resulting in a safe teardown.

### logan server
logan server connects to one or many logan clients and stores all received metrics in a SQLite3 database. It is also able to listen to logging produces by the `ModelLogger` from the [Runtime Environment](https://github.com/cdit-ma/re).

### logan client
logan client utilises the sigar system monitoring interface to collect system metrics and ZeroMQ to communicate with one or many logan servers. Logging can be run in live or cached mode to optimise for either network traffic or disk I/O. Logging frequency can also be specified to control data point granularity.

## Minimum software requirements:
* C++11 compatible compiler
* [CMake 3.0](https://cmake.org/)
* [libzmq](https://github.com/zeromq/libzmq)
* [Google Protobuf](https://github.com/google/protobuf)
* [Sigar](https://github.com/hyperic/sigar)

## Optional software requirements
* [Ninja](https://ninja-build.org/)

## Build Instructions:
```
git clone https://github.com/cdit-ma/logan --recursive
cd MEDEA
mkdir build
cd build
cmake .. -G "Ninja"
cmake --build .
```

### Notes:
* Run `cmake -G` to list all generators supported for your OS (ie `"Unix Makefiles"` for `Unix`)

### Windows Notes:
* Add `-DCMAKE_BUILD_TYPE=Release` to the cmake generator command
* Add `--config Release` to the cmake build command

### CMake Options:
* `-DBUILD_CLIENT=ON/OFF` - Toggles building of logan_client
* `-DBUILD_SERVER=ON/OFF` - Toggles building of logan_server
* `-DDISABLE_MODEL_LOGGING` - Disables logan_server's model logging capibilities 
* `-DDISABLE_HARDWARE_LOGGING` - Disables logan_server's hardware logging capibilities


## Usage

### Server command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
| -c, --clients [arg list]              | List of logan_client endpoints to connect to|
| -d, --database [arg]                  | Filename of output database  |

### Client command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help.                   |
| -s, --system_info_print               | Print system info then exit           |
| -l, --live_mode [arg (=0)]            | Produce data live                     |
| -P, --process [arg list]              | Monitor specific processes            |
| -f, --frequency [arg (=1)]            | Logging frequency in Hz               |
| -p, --publisher [arg]                 | Publisher endpoint (ie tcp://192.168.111.1:5555)|


## Example Usage
Running a simple logging scenerio on localhost. These commands can be run in different shells, and `CTRL+C` can be used to interupt instead of the `killall command`

```
cd logan/bin
./logan_server -c tcp://127.0.0.1:7001 tcp://127.0.0.1:7002 -d output.sql &
./logan_client -p tcp://127.0.0.1:7001 -f 0.5 -P logan_server &
./logan_client -p tcp://127.0.0.1:7002 -f 2 --live_mode true &
killall logan_client --wait
killall logan_server --wait
```