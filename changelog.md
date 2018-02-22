# re v1.5.0

## New Features:
* Fixed bug in DDS implementations which caused messages to be occasionally missed
* Added re_gen as a submodule
* Updated Re to handle RTI DDS 5.3.0
* Fixed interoperability issues between DDS implementations

## Submodule Updates:
* Updated re_gen to v1.5.2

## JIRA Issues:
|	Issue key	|	Summary	|
|	--- | --- |
|   [RE-164](https://cdit-ma.atlassian.net/browse/RE-164)   |   Not Interoperability between RTI->OSPL |
|   [RE-163](https://cdit-ma.atlassian.net/browse/RE-163)   |   Un-Initialised EnumMembers sporadically stop RTIDDS messages from being sent/recieved |
|   [RE-158](https://cdit-ma.atlassian.net/browse/RE-158)   |   Missing every 10 messages roughly with RTI, when on differing nodes 
|   [RE-147](https://cdit-ma.atlassian.net/browse/RE-147)	|   Update re to use RTI DDS 5.3.0	|
|   [RE-129](https://cdit-ma.atlassian.net/browse/RE-129)   |   Pure Virtual exception getting called in event port queue model |
|   [RE-105](https://cdit-ma.atlassian.net/browse/RE-105)   |   Relative paths for lib in re_node_manager -l option |

---

# re v1.4.0

## New Features:
* Fixed model parsing issues which made loading large models slow
* Adjusted network code communication between master/slave to speed up the standup of executions
* Added checks to ensure that all nodes are configured before the Activate message is sent
* Various bug fixes and optimizations

## Testing:
* Added test_rti
* Added test_zmq
* Added test_qpid
* Added test_ospl

## JIRA Issues:
|	Issue key	|	Summary	|
|	--- | --- |
|	[RE-114](https://cdit-ma.atlassian.net/browse/RE-114)	|	Enable Logging in InEventPort/OutEventPort class	|
|	[RE-115](https://cdit-ma.atlassian.net/browse/RE-115)	|	Add Attribute Factory in Activatable class to save code bloat	|

---

# re v1.3.2

## New Features:
* Periodic Events have been refactored to improve accuracy and timing of events
* Improved Queueing mechanism of InEventPorts

## Testing:
* Added Google Test as testing framework
* Tests can be built by adding -DBUILD_TEST ON (defaults to false)
* Added test_periodicevent
* Added test_cpuworker
* Added a jenkinsFile to run simple model tests

## JIRA Issues:
|	Issue key	|	Summary	|
|	--- | --- |
|	[RE-102](https://cdit-ma.atlassian.net/browse/RE-102)	|	Create a regression test job to compile and run the re tests/build process	|
|	[RE-100](https://cdit-ma.atlassian.net/browse/RE-100)	|	Add all commands/options + examples for execution of all programs into README.md	|
|	[RE-98](https://cdit-ma.atlassian.net/browse/RE-98)	|	Update model parser to pull out model comment.	|
|	[GEN-36](https://cdit-ma.atlassian.net/browse/GEN-36)	|	Cleanup JenkinsFiles across all projects and use standard naming schema	|
|	[GEN-30](https://cdit-ma.atlassian.net/browse/GEN-30)	|	Update run model tests jenkins job to recurse subdirs.	|
|	[RE-97](https://cdit-ma.atlassian.net/browse/RE-97)	|	Add tutorial models to test_models repo.	|
|	[RE-92](https://cdit-ma.atlassian.net/browse/RE-92)	|	Add a changelog.md file with the list of changed features	|
|	[RE-96](https://cdit-ma.atlassian.net/browse/RE-96)	|	Periodic Event should sleep for the correct length of time	|
|	[RE-99](https://cdit-ma.atlassian.net/browse/RE-99)	|	Integrate Google Test with RE	|
|	[RE-87](https://cdit-ma.atlassian.net/browse/RE-87)	|	Build jenkins job to test re with a series of models	|
|	[RE-51](https://cdit-ma.atlassian.net/browse/RE-51)	|	Test RE on windows	|

---

# re v1.3.1

## New Features:
* Added support for Enumerations
* Components are not Activated until all Components have finished running StartUp
  * This should ensure that events are not lost at the start of execution

## Bug Fixes:
* Fixed bugs with termination of re_node_manager
* Fixed Attributes not supporting all primitive types.

---

# re v1.1.0

## New Features:
Relative to previous solutions, RE has several new features.
* Dynamic Component compilation
    * Components requiring unsupported middlewares are no longer compiled
    * Components are now built as dynamic libraries
    * Components that are not deployed are no longer loaded into the runtime
* Dynamic runtime
    * Model attributes can now be modified by external tools during runtime
    * Components can now be dynamically activated/deactivated during execution
* Performance
    * Redesigned from the ground up to have minimal execution overhead
    * Lightweight middleware (ZMQ) used for model execution control
* Model Logging
    * Logging of model events is now built into the runtime environment. This can be run in either live or cached mode (or disabled entirely)
* C++11 support
    * RE exploits many language features new to C++11, resulting in an easier to read and maintain codebase.