# Runtime Environment v3.2.9
* Increased timeout duration for the SlaveStartup function in Slave->Master re_node_manager communication
* Added is_master field to the NodeDeployment proto message
* Updated deploy_model script:
  * Preferencially launches re_node_manager(master) and logan_server before slaves
  * Runs model validation only on compilation/execution failure
  * Only unstashes one set of library binaries per node


## Resolved JIRA Tickets:
* RE-386
* RE-392
* RE-395
* RE-396
* RE-397
* LOG-89
* REGEN-83
* REGEN-84
