# re_gen v1.5.2

## New Features:
* Updated to match the latest changes in re v1.5.0

## Validation:
* Added check for use of C++ reserved words RE class names for Aggregates, Members, Components

## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [REGEN-44](https://cdit-ma.atlassian.net/browse/REGEN-37) | Catch Member names not being unique when compared to parent Aggregate |
| [REGEN-43](https://cdit-ma.atlassian.net/browse/REGEN-43) | Check for case-insensitive Aggregate labels/namespaces being the same |
| [REGEN-42](https://cdit-ma.atlassian.net/browse/REGEN-42) | Place a version number into generated code |

---

# re_gen v1.5.1

## New Features:
* Added a sparse mode boolean flag to only compile Aggregates/Components which are actually being deployed (defaults to true)
* Fixed a vast list of bugs to do with duplicated named objects
* Added initialization to all primitives declarations


## Validation:
* Added checks to handle enum issues and duplicate symbol warnings
* Added check for invalid/reserved labels of Aggregates/Enumerations
* Added check for Enum with no defined EnumMembers 


## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [RE-163](https://cdit-ma.atlassian.net/browse/RE-163) |Un-Initialised EnumMembers sporadically stop RTIDDS messages from being sent/recieved |
| [REGEN-37](https://cdit-ma.atlassian.net/browse/REGEN-37) | Catch Member names not being unique when compared to parent Aggregate |
| [REGEN-38](https://cdit-ma.atlassian.net/browse/REGEN-38) | Enums defined expect capitals for the label |
| [REGEN-39](https://cdit-ma.atlassian.net/browse/REGEN-39) | Make .proto and .idl files encapsulate namespace into the file name |
| [REGEN-40](https://cdit-ma.atlassian.net/browse/REGEN-40) | Compilation error when dds attempts to send vector of strings |
| [REGEN-41](https://cdit-ma.atlassian.net/browse/REGEN-41) | Implement a sparse generation mode to only compile the data types we need |

---

# re_gen v1.5.0f

## New Features:
* Rewrote code-gen
* Improved performance by leveraging xsl:key functionality for caching lookups
* Added documentation into transforms
* Seperated language specific generation/functions into seperate XSL files
* Updated codegen to reflect changes in re v1.4.0
* Variables now generated in a consistent manner to attributes, with setters and getters. Variable can still be used directly, using the underscore-suffix name. Below is the code generated for a Variable of type Int, labelled count.
    ```
    public:
        // [Variable] count <189>
        void set_count(const int& value);
        const int& get_count() const;
        int& count();
    private:
        int count_;
    ```
* Variadic parameters are no longer explicitly casted to doubles, meaning that the Utility_Workers->Log function is able to correctly display ints/doubles/floats independently.
* Improved readability of generated code
* Code-gen now uses shared_ptr for workers
* Cleaned up CMakeLists.txt generation to use target_include_directories and target_compile_definitions


## Validation:
* Updated the rule which checks for unset parameters in ComponentImpl's. Rule now checks to see if all Parameters (Input, Variable, Variadic) have either a value set, or a data-linked edge
* Added validation check for Utility Workers Log function to check for mix-match of parameter types to % arguments


## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [RE-126](https://cdit-ma.atlassian.net/browse/RE-126) | Support DDS middlewares for all types|
| [REGEN-32](https://cdit-ma.atlassian.net/browse/REGEN-32) | Update re_gen to latest branch |
| [REGEN-33](https://cdit-ma.atlassian.net/browse/REGEN-33) | Add Validation to Utility Worker Log function to match parameter types |
| [REGEN-34](https://cdit-ma.atlassian.net/browse/REGEN-34) | RTI Generation fails on Enum setting in convert.cpp |
| [REGEN-35](https://cdit-ma.atlassian.net/browse/REGEN-35) | Protobuf is failing to link against debug/optimize library |
| [REGEN-36](https://cdit-ma.atlassian.net/browse/REGEN-36) | Variable of a vector of Aggregates fails to include the relevant header |

---

# re_gen v1.4.0

## 
* Handles Printf properly now


## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [REGEN-26](https://cdit-ma.atlassian.net/browse/REGEN-26) | Add a changelog.md file with the list of changed features |
| [REGEN-27](https://cdit-ma.atlassian.net/browse/REGEN-27) | Having a Vector of an AggregateInstance in an Aggregate doesn't compile |

---

# re_gen v1.3.2

## Resolved JIRA Issues:
| Issue key | Summary|
| --- | --- |
| [REGEN-26](https://cdit-ma.atlassian.net/browse/REGEN-26) | Add a changelog.md file with the list of changed features |
| [REGEN-27](https://cdit-ma.atlassian.net/browse/REGEN-27) | Having a Vector of an AggregateInstance in an Aggregate doesn't compile |

---