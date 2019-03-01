# Runtime Environment v3.3.4
* The Worker interface now to requires a get_version() to be implemented in Sub-Classes
  * This is used for tying versions of Workers with MEDEAs *.worker.graphml definitions files

## Workers
* Updated all current workers to have an appropriate version
### Dis_Worker v0.1.0
Added Dis_Worker to allow communication with the Distributed Interactive Simulation (DIS)
* Implemented using KDIS in DISv6
* Added a connect() function which connects to a broadcast address / port (driven from Attributes)
* Added a disconnect() function which disconnects from the DIS network
* Added a PDU2String() function which returns a human-digestable string representation of a PDU
* Implemented a Callback registration function, used by re_gen to attach a custom MEDEA function to process received PDU objects

## Resolved JIRA Tickets:
* RE-435
* RE-434