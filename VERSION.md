# MEDEA v3.3.5-VIS
## New Features:
* Added a setting to enable the Worker Aspect
* Added the ability to dynamically change PeriodicPort's frequency in ComponentImpl
* Enforced MEDEA to use SemVer versioning standards for both MEDEA's versions and Worker/Class Versions
* MEDEA now sets a default version of '0.0.0' to Class entities, which is also displayed visually on ClassInstance entities
* Added an 'worker' icon which is rendered over the icon on ClassInstance's which are known to MEDEA as Workers
* Added preliminary visualisation for select logged experiment data (BETA)

## Workers:

## Tests:

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
