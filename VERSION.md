<<<<<<< HEAD
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
=======
# MEDEA v3.3.9
>>>>>>> develop

## Workers:
* Fixed FFT FPGA hanging bug, now safe to run with vectos of 8192 floats (4096 complex pairs)

## Tests:
* No updates

## Examples:
* No updates

## Bug Fixes:
* Fixed code gen issue preventing project containing OpenCL Worker from building with C++17 features

## Resolved JIRA Tickets:
<<<<<<< HEAD
* RE-456
* RE-437
* RE-436
* RE-127
* MED-540
>>>>>>> develop
=======
* RE-468
* REGEN-99
>>>>>>> develop
