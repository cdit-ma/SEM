# Runtime Environment v3.3.4
* The Worker interface now to requires a get_version() to be implemented in Sub-Classes
  * This is used for tying versions of Workers with MEDEAs *.worker.graphml definitions files
* Added human readable error string when pugixml fails to parse the graphml document
* Updated Jenkins deploy_model script to support docker
* Added templated Setters/Getters for Attributes, exposed templatate Setter/Getters in Activatable
* Added GetFrequency in the PeriodicPort to allow dynamic modification in MEDEA

## Workers
### Cpu_Worker v1.1.0
* Added version number
### Memory_Worker v1.0.0
* Added version number
### OpenCL_Worker v1.2.0
* Added version number
### Utility_Worker v1.3.0
* Added version number
### Dis_Worker v0.1.0
* Added Dis_Worker to allow communication with the Distributed Interactive Simulation (DIS)
* Implemented using KDIS in DISv6
* Added a connect() function which connects to a broadcast address / port (driven from Attributes)
* Added a disconnect() function which disconnects from the DIS network
* Added a PDU2String() function which returns a human-digestable string representation of a PDU
* Implemented a Callback registration function, used by re_gen to attach a custom MEDEA function to process received PDU objects

## Resolved JIRA Tickets:
* RE-406
* RE-434
* RE-435
* RE-443
* MED-524