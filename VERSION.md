<<<<<<< HEAD
# MEDEA v3.3.5-VIS
## New Features:
* Added visualisation for Marker events
* CPU and memory utilisation data are now charted as line graphs
* Chart queries can now be sent without having to waiting for previous reuqest responses
* Added functionality that allows the user to trigger and filter chart data by kind by right-clicking on the aspects

## Bug Fixes:
* Re-implemented the ability to query chart data by paths as graphml IDs may be inconsistent
* Fixed chart popup hanging when there is an error with the connection to the Aggregation broker

## Resolved JIRA Tickets:
* MED-534
* MED-551
* MED-553
* MED-557
* MED-558
* MED-559
* MED-562
* MED-556
=======
# MEDEA v3.3.8

## Workers:
* Added TCP worker (v0.1.0)
* Updated opencl.worker.graphml to v1.3.0
  * Added new icons for OpenCL Worker
  * Updated and replaced old icons
  * Fixed bug with KMeans not compiling
  * Updated OpenCL Buffer to be in new namespace

## Tests:
* Add test/example model to catch custom class related build fails

## Examples:
* Added tcp_example.graphml
* Added sensor_array.graphml tutorial

## Bug Fixes:
## Resolved JIRA Tickets:
* RE-456
* RE-437
* RE-436
* RE-127
* MED-540
>>>>>>> develop
