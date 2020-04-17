# Runtime Environment v3.3.9

## Middlewares
* Forked KDIS in order to amend source files to support building with modern C++ compilers

## Workers
* Fixed TCP Worker Bug

## Logging
* Added "--init" flag to AggregationServer to clear & initialise the database it connects to (LOG-135)
* Added support for logging PortConnections as part of an experiment's state (PR #148)
* Added support for querying aggregation databse for PortEvents (SEM-10, SEM-11, SEM-17) 
* LifecycleEvent type names updated in protobuf API (LOG-145)
* Added Protobuf API support for NetworkUtilisationEvents (SEM-83)

## Code Generation
* Fixed incorrect datatype generation in TAO server interface generation (PR #145)

## Jenkins
* Middleware environment variables are now determined by environment variables in the re_configure script for docker (PR #146)
* Throttle raspberry Pi builds to only use 2 cores, this avoids a compile time crash (PR #150)

## Environment Manager
* Running a build with offline nodes should now lo longer cause a crash (RE-442)

## Project structure
* Submoduled CDIT-MA projects collapsed into the main RE project (PR #142)
* External projects that are built from source are now being migrated to the extern subdirectory
* Added dependency on libpqxx
* Added a dependency on libcurl (SEM-74)
* Added dependency on [DockerClient](https://github.com/cqbqdd11519/DockerClient) cpp API (SEM-74)
* Doxygen build support has been added (GEN-164)
* Removed KDIS from source tree (GEN-167)
* Moved to MIT license for top level project, removed Logan LGPL licensing (PR #149)
* Fixed CMake project versioning hierarchy semantics (Internal change only) (PR #158)