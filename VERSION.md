# Runtime Environment v3.3.5
* Updated Logger::GetCurrentTime() to utilize microsecond level precision
* Updated ProtoWriter Monitor Interface to fix issues with Zmq::ProtoWriter's never terminating

## Workers
### Utility_Worker v1.3.1
* Refactored the utility worker to utilize the Logger::GetCurrentTime()

## Code Generation
* Improved generation of periodic event port frequency mutation.

## Resolved JIRA Tickets:
* RE-446
* RE-440

