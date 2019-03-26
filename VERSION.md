# Runtime Environment v3.3.5-1
* Updated Logger::GetCurrentTime() to utilize microsecond level precision
* Updated ProtoWriter Monitor Interface to fix issues with Zmq::ProtoWriter's never terminating
* Fixed some bugs in code-gen

## Workers
### Utility_Worker v1.3.1
* Refactored the utility worker to utilize the Logger::GetCurrentTime()

## Resolved JIRA Tickets:
* RE-446
* RE-440
* RE-450
* LOG-124
* RE-445
* RE-448