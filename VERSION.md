# Runtime Environment v3.3.5-1
* Updated Logger::GetCurrentTime() to utilize microsecond level precision
* Updated ProtoWriter Monitor Interface to fix issues with Zmq::ProtoWriter's never terminating

## Logan
* Fixed Logan server not correctly storing int64's in the database, which made tx/rx packets overflow

## Workers
### Utility_Worker v1.3.1
* Refactored the utility worker to utilize the Logger::GetCurrentTime()

## Code Generation
* Improved generation of periodic event port frequency mutation.

## Resolved JIRA Tickets:
* RE-440
* RE-445
* RE-446
* RE-448
* RE-450
* LOG-124
