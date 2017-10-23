# re v1.3.2

## New Features:
* Periodic Events have been refactored to improve accuracy and timing of events
* Improved Queueing mechanism of InEventPorts

## Testing:
* Added Google Test as testing framework
* Tests can be built by adding -DBUILD_TEST ON (defaults to false)
* Added a PeriodicEventPort tester

## JIRA Issues:
* RE_96 RE_99 RE_87 RE-92

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