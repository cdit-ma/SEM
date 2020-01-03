# MEDEA v3.4.0
## New Features:
* Added charts for Port events
* Added Pulse panel 
    * A view that displays experiment state and allows playback of Port lifecycle and Port events for a selected experiment run
    * Includes live-polling capability
* Added a selection at the bottom of the chart popup (used to select the experiment run) for which type of visualisation the experiment run data should be displayed
* Separated live experiments from finished experiments in the chart popup 

## Visual Bugs:
In the process of upgrading Qt version, it's been noticed that several theme bugs exist. It has been hard to comprehensively identify all of these bugs. We have fixed the ones we have found; if any further bugs are experienced please report them. 

## New Dependencies:
* Changed active development to Qt version 5.12.14 
* Added a dependency on Protobuf 3.6
* Added dependency on ZeroMQ 4.2.5
* Added entire RE repository as a submodule to replace re_common submodule