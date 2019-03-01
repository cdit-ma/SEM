# MEDEA v3.3.4
## New Features:
* Added a setting to enable the Worker Aspect
* Added the ability to dynamically change PeriodicPort's frequency in ComponentImpl
* Enforced MEDEA to use SemVer versioning standards for both MEDEA's versions and Worker/Class Versions
* MEDEA now sets a default version of '0.0.0' to Class entities, which is also displayed visually on ClassInstance entities
* Added an 'worker' icon which is rendered over the icon on ClassInstance's which are known to MEDEA as Workers

## Workers:
* Added a new Dis_Worker.worker.graphml v0.1.0
* Updated Cpu_Worker.worker.graphml to to v1.1.0
* Updated Memory_Worker.worker.graphml to v1.0.0
* Updated OpenCL_Worker.worker.graphml to v1.2.0
* Updated Utility_Worker.worker.graphml to v1.3.0
* Updated Dis_Worker.worker.graphml to v0.1.0

## Bug Fixes:
* Fixed the syntax highlighter being greedy with double quotes ""

## Resolved JIRA Tickets:
* MED-539
* MED-535
* MED-538
* MED-524
