# MEDEA v3.2.5
## New Features:
* Updated re_gen to v3.2.5
* Added new DeploymentContainer entity to support containerised deployment. 
 * ComponentInstances/ComponentAssemblies/LoggingProfile can be deployed to DeploymentContainers, which are mapped to an unique instance of re_node_manager.
 * DeploymentContainers should be deployed to HardwareNode/HardwareClusters, in a many-one relationship
* Updated Connect menu to show edge direction title label
* Updated some older icons

## Validation:
* Added a check for undeployed DeploymentContainers