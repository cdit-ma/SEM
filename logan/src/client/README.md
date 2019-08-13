# logan client
logan client utilises the sigar system monitoring interface to collect system metrics and ZeroMQ to communicate with one or many logan servers. Logging can be run in live or cached mode to optimise for either network traffic or disk I/O. Logging frequency can also be specified to control data point granularity.

## Building
**Requires:**
* https://github.com/google/protobuf
* https://github.com/zeromq/libzmq
* https://github.com/hyperic/sigar
* https://github.com/cdit-ma/re_common

**Installation:**
See logan root for build instructions.