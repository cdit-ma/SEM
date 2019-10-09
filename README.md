# re
"Runtime Environment"

**_re_** aims to provide users with an easy to use runtime environment for system modeling. 
**_re_** is best used in conjunction with [MEDEA](https://github.com/cdit-ma/MEDEA.git) for graphical modeling.


## Supported Operating Systems:
* Windows 7 (> Visual Studio 2015)
* MacOSX 10.10 (Or Newer)
* Ubuntu 16.04 (or Newer)

## Requirements
### Runtime
* [Google Protobuf](https://github.com/google/protobuf)
* [ZeroMQ](https://github.com/zeromq/libzmq)
* [Boost](https://www.boost.org) - Program\_Options, System, Filesystem and Date\_Time
* [PugiXML](https://pugixml.org)
* [Saxon XSLT Processor](http://saxon.sourceforge.net/)
* [CMake](https://github.com/Kitware/CMake)
* A C++17 compliant compiler
### Logging
* libpqxx-dev
* [SIGAR](https://github.com/cdit-ma/sigar)

## Installation
See [Installation Guide](INSTALL.md)

## License
This project is licensed under the terms of the [MIT license](LICENSE.txt).

## Support
This work has been developed with the support of the Australian Government Defence Science and Technology Group.

## Usage
### Command line options
| Flag                                  | Description                           |
|---------------------------------------|---------------------------------------|
| -h, --help                            | Displays this help                    |
| -d, --deployment [arg]                | Path to model GraphML file            |
| -l, --library [arg]                   | Path to runtime libraries             |
| -t, --time [arg]                      | Deployment duration (in seconds)      |
| -s, --slave [arg]                     | Slave endpoint, including port        |
| -m, --master [arg]                    | Master endpoint, including port       |
| -L, --live-logging [arg]              | Enables model logger live logging     |


### Example Usage:
Example of simple 10 second, two node, model execution with live model logging on.
```
>Generate datatypes (run in intended output directory eg /home/cdit-ma/runtime)
java -jar /home/cdit-ma/re_gen/saxon.jar -xsl:/home/cdit-ma/re_gen/g2qos.xsl -s:/home/cdit-ma/models/model.graphml middlewares=ZMQ,PROTO

>Generate components (run in intended output directory eg /home/cdit-ma/runtime)
java -jar /home/cdit-ma/re_gen/saxon.jar -xsl:/home/cdit-ma/re_gen/g2components.xsl -s:/home/cdit-ma/models/model.graphml middlewares=ZMQ,PROTO

>Build runtime libraries (run in intended output directory eg /home/cdit-ma/runtime/libraries)
cmake ..
make -j6

>Prior two steps must be done on all nodes used by model execution

>Start re master (on node 192.168.111.100)
./re_node_manager -m tcp://192.168.111.100:7000 -l /home/cdit-ma/runtime/libraries -t 10 -d /home/cdit-ma/runtime/models/model.graphml -L true

>Start re slave (on node 192.168.111.101)
./re_node_manager -s tcp://192.168.111.101:7000 -l /home/cdit-ma/runtime/libraries
```
Example of single node execution.
```
>Assume all code generation and compilation outlined in previous example is complete.
./re_node_manager -m tcp://192.168.111.100:7000 -s tcp://192.168.111.100:7001 -l /home/cdit-ma/runtime/libraries -t 10 -d /home/cdit-ma/runtime/models/model.graphml -L true
```
