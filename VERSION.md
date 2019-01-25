# MEDEA v3.3.2
## New Features:
* Added new data 'column_count' to allow the user to grid align Aggregate, Enums
* Moved 'SharedDataType' and 'Namespace' entities to use grid alignment, customizable with 'column_count'
* Updated and added new 'Replication' attribute inside ComponentAssembly which replaces the old 'replication_count' data element
  * The Replication attribute can be connected to DeploymentAttributes (Of numeric types) and other 'Replication' attributes.
  * The 'value' can also be set hard coding the replication to that number
  * Values less than 0 effectively disables the ComponentAssembly

## Resolved JIRA Tickets:
* MED-467
* MED-355