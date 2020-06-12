# logan v1.3.2

## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [LOG-26](https://cdit-ma.atlassian.net/browse/LOG-26) | Add all commands/options + examples for execution of all programs into README.md |
| [LOG-25](https://cdit-ma.atlassian.net/browse/LOG-25) | Add a changelog.md file with the list of changed features |

---

# logan v1.3.1

## Bug Fixes:
* Logan handles Port Kind correctly in the table model events

---

# logan v1.1.0

## New Features:
* Logan handles model events
    * Model events include:
         * Ports firing
         * Component activation/passivation
         * Workloads starting and ending execution
         * User defined model events
         * And many other model driven events
* Improved modularity of codebase
    * Components of logan can now be disabled at compile time
        * Users have the ability to disable model or hardware logging (or both)
        * Client and server compilation be individually targetted
        * Server and client extensibility has been improved. Dropping in new logging components is now a simple process.
* Improved performance
    * Logan has been entirely reworked to use the lightweight ZMQ middleware, reducing both build requirements and memory/cpu overhead
    * Database insertion logic has been improved, resulting in scalability improvements
    * Database management is now done through the sql amalgamation. This reportedly provides a 5-10% performance gain (https://www.sqlite.org/amalgamation.html)
* Improved support for multi-server logging solutions
