# MEDEA v3.0.0
## New Features:
* New containment workload paradigm
  * Interface:
    * Namespace
    * Server Interface
  * Behaviour:
    * Class
    * Function
    * Class Instance
    * Function Call
    * For Loop
    * While Loop
    * Boolean Expression
    * Setter
    * If Statement
    * Input Parameter Group
    * Return Parameter Group
    * Server Port Impl
    * Server Request
    * External Type
  * Hardware:
    * OpenCL Platform
    * OpenCL Device
  * Assembly:
    * Deployment Attribute
    

* Updated rendering code for Nodes
    * Broken into cells ordered by rows/columns
    * Color palette driven by active theme
    * Visual representation matches model order
    * Automatic resizing
    * Elements can be visually re-ordered which updates the model
    * Tooltips and hover-visuals implemented
    * New visual indicator for expand/contract
* Visual Edge 'knoblets'
  * Connect/Disconnect Menu available via clicking (Context/Direction specific menu)
  * Visual connecting by click-dragging
  * 'knoblet' position is directionally contextual, and determined by valid edges
  * Multiple connection when group is selected possible
  * Used for all Edge types
  * Visual highlight when edge is connected
  * Contracting a Node will route its children edges through the first visible parent's knoblets
* Visual Notification 'knoblets'
  * Nodes which have Notifications attached will show validation notifications on canvas
  * Grouped by severity.
  * Contracting a Node will pass its children notifications to its first visible parent
  * Clicking on a notification will open the Notification Panel and only show the notifications for the selected entities




## Resolved JIRA Issues:
| Issue key | Summary  |
|-----------|----------|
| [MED-319](https://cdit-ma.atlassian.net/browse/MED-319)	| Add an option to request only jobs that the logged in user logged |


# MEDEA v2.4.2
## New Features:
* Updated Execution Manager
* Added a setting to enable user-based job querying
* Changed Jenkins Manager to use new std::async requests and futures
* Updated re_gen to v1.5.1b

## Resolved JIRA Issues:
| Issue key | Summary  |
|-----------|----------|
| [MED-319](https://cdit-ma.atlassian.net/browse/MED-319)	| Add an option to request only jobs that the logged in user logged |

# MEDEA v2.4.1
## New Features:
* Updated Execution Manager
  * Added a refresh Jenkins Job button to query recent jobs
  * Added an Artifacts panel, to show Jenkins jobs artifacts when jobs are complete
  * Panel auto queries recent jobs after a build job is run
* Integrated file upload parameter changes to Build Job request
* Updated re_gen to v1.5.0g

## Resolved JIRA Issues:
| Issue key | Summary  |
|-----------|----------|
| [MED-311](https://cdit-ma.atlassian.net/browse/MED-311)	| Implement file parameter uploading in MEDEA for Jenkins |

# MEDEA v2.4.0
## New Features:
* Added a MEDEA_cli shell program which allows command line import/export of models
* Added test frameworks for components:
  * test_idlparser - Runs test cases for the idl2graphml parser
  * test_modelcontroller - Runs basic model importing tests on the ModelController class
## Resolved JIRA Issues:
| Issue key | Summary  |
|-----------|----------|
| [MED-276](https://cdit-ma.atlassian.net/browse/MED-276)	| Ability to place enum within a shared data type | |
| [MED-275](https://cdit-ma.atlassian.net/browse/MED-275)	| Allow multiple IDL files to be passed to be passed into the IDL Parser |
| [MED-273](https://cdit-ma.atlassian.net/browse/MED-273)	| idl import stops when it encounters an interface |
| [MED-272](https://cdit-ma.atlassian.net/browse/MED-272)	| #include functionality when parsing idl files |
| [MED-271](https://cdit-ma.atlassian.net/browse/MED-271)	| Move test folder outside of src |
| [MED-270](https://cdit-ma.atlassian.net/browse/MED-270)	| Add all commands/options + examples for execution of all programs into README.md |
| [MED-269](https://cdit-ma.atlassian.net/browse/MED-269)	| Fix Local Deployment on Ubuntu |
| [MED-268](https://cdit-ma.atlassian.net/browse/MED-268)	| Add Dans Laptop as a OSX build node |
| [MED-267](https://cdit-ma.atlassian.net/browse/MED-267)	| Setup Windows VM for release building |
| [MED-266](https://cdit-ma.atlassian.net/browse/MED-266)	| Create Jenkins job for building MEDEA |
| [MED-264](https://cdit-ma.atlassian.net/browse/MED-264)	| Implement CLI MEDEA |
| [MED-262](https://cdit-ma.atlassian.net/browse/MED-262)	| Can not rename label of index variable in for condition while loop |
| [MED-261](https://cdit-ma.atlassian.net/browse/MED-261)	| Setup toggle icons for the tabs in the vizualisation panel. |
| [MED-260](https://cdit-ma.atlassian.net/browse/MED-260)	| Integer variable (i in default case) in for condition while loop can not be connected from (data edge) |
| [MED-258](https://cdit-ma.atlassian.net/browse/MED-258)	| Implement extension of addTab |
| [MED-255](https://cdit-ma.atlassian.net/browse/MED-255)	| Implement tab-able panel for visualization graphs |
| [MED-254](https://cdit-ma.atlassian.net/browse/MED-254)	| Add comment field to model |
| [MED-251](https://cdit-ma.atlassian.net/browse/MED-251)	| idl parsing should handle exceptions |
| [MED-247](https://cdit-ma.atlassian.net/browse/MED-247)	| Fix IDL Parser to handle block comments with non-greedy regex |
| [MED-246](https://cdit-ma.atlassian.net/browse/MED-246)	| Add all valid IDL primitive types to IDL parser |
| [MED-245](https://cdit-ma.atlassian.net/browse/MED-245)	| Build MEDEA rollout job on Jenkins |
| [MED-243](https://cdit-ma.atlassian.net/browse/MED-243)	| Add a changelog.md file with the list of changed features |

# MEDEA v2.3.3
## New Features:
* Added a MEDEA_cli shell program which allows command line import/export of models
* Added test frameworks for components:
  * test_idlparser - Runs test cases for the idl2graphml parser
  * test_modelcontroller - Runs basic model importing tests on the ModelController class
## Resolved JIRA Issues:
| Issue key | Summary  |
|-----------|----------|
| [MED-276](https://cdit-ma.atlassian.net/browse/MED-276)	| Ability to place enum within a shared data type | |
| [MED-275](https://cdit-ma.atlassian.net/browse/MED-275)	| Allow multiple IDL files to be passed to be passed into the IDL Parser |
| [MED-273](https://cdit-ma.atlassian.net/browse/MED-273)	| idl import stops when it encounters an interface |
| [MED-272](https://cdit-ma.atlassian.net/browse/MED-272)	| #include functionality when parsing idl files |
| [MED-271](https://cdit-ma.atlassian.net/browse/MED-271)	| Move test folder outside of src |
| [MED-270](https://cdit-ma.atlassian.net/browse/MED-270)	| Add all commands/options + examples for execution of all programs into README.md |
| [MED-269](https://cdit-ma.atlassian.net/browse/MED-269)	| Fix Local Deployment on Ubuntu |
| [MED-268](https://cdit-ma.atlassian.net/browse/MED-268)	| Add Dans Laptop as a OSX build node |
| [MED-267](https://cdit-ma.atlassian.net/browse/MED-267)	| Setup Windows VM for release building |
| [MED-266](https://cdit-ma.atlassian.net/browse/MED-266)	| Create Jenkins job for building MEDEA |
| [MED-264](https://cdit-ma.atlassian.net/browse/MED-264)	| Implement CLI MEDEA |
| [MED-262](https://cdit-ma.atlassian.net/browse/MED-262)	| Can not rename label of index variable in for condition while loop |
| [MED-261](https://cdit-ma.atlassian.net/browse/MED-261)	| Setup toggle icons for the tabs in the vizualisation panel. |
| [MED-260](https://cdit-ma.atlassian.net/browse/MED-260)	| Integer variable (i in default case) in for condition while loop can not be connected from (data edge) |
| [MED-258](https://cdit-ma.atlassian.net/browse/MED-258)	| Implement extension of addTab |
| [MED-255](https://cdit-ma.atlassian.net/browse/MED-255)	| Implement tab-able panel for visualization graphs |
| [MED-254](https://cdit-ma.atlassian.net/browse/MED-254)	| Add comment field to model |
| [MED-251](https://cdit-ma.atlassian.net/browse/MED-251)	| idl parsing should handle exceptions |
| [MED-247](https://cdit-ma.atlassian.net/browse/MED-247)	| Fix IDL Parser to handle block comments with non-greedy regex |
| [MED-246](https://cdit-ma.atlassian.net/browse/MED-246)	| Add all valid IDL primitive types to IDL parser |
| [MED-245](https://cdit-ma.atlassian.net/browse/MED-245)	| Build MEDEA rollout job on Jenkins |
| [MED-243](https://cdit-ma.atlassian.net/browse/MED-243)	| Add a changelog.md file with the list of changed features |

---

# MEDEA v2.3.1
## New Features:
### General:
* Fixed keyboard shortcuts
* Implemented Local Deployment
* Implemented Import IDL functionality (#125, #126, #127, #128, #129, #130)
* Added an idl2graphml shell program to do idl conversion outside of MEDEA
* Fixed installer to have a more appropriate installer directory
* Reverted to old Minimap (#116)
* Various visual and icon changes

### New Dock/Context Menus:
* Menus are now filterable (#121)
* Menus which contain large numbers of elements sparsely load the first 10 entities, with the ability to load more.

### Edges:
* Click and drag connections in the canvas are now directional
  * Edges can be constructed from destination -> source and source -> destination
  * Edges respect their connections direction
    * Edges from the left = Incoming
    * Edges from the right = Out going
  * When dragging a connection, arrow heads are drawn at the appropriate end to denote direction visually
* Edges are now a dark shade of gray to improve visibility
* Edges thickness is based on whether the terminal entity is visible (Thick if visible, Thin if entity is collapsed in a parent)
* Added support for multiple edge connections to be constructed via the canvas
  * Added support for multiple connect points
  * Collapsed parents will display children's edges combined
* Single clicking connect points will open a contextual connect menu.


### Notifications:
* Added SUCCESS and RUNNING severities
* Clicking the Notification Status bar icons now opens the Notification Panel with respective filter applied
* Notification Dialog uses lazy loading
  * Scrolling to the bottom of the panel will automatically load more items.
* Selecting notifications in the Notification Dialog will highlight the attached entity
* Split notifications to have a title and description
  * Notification Popup only toasts the title of the notification
  * Notification Panel will show both title and description
* Fixed various layout issues in Notification Panel
* Clicking the Notification Popup will instantly hide it
* Added a remove button in the Notification Panel to remove all notifications which are currently filtered
* Added sorting of the Notification Panel via update time (Ascending/Descending)

### Search:
* Notification Dialog uses lazy loading
  * Scrolling to the bottom of the panel will automatically load more items.
* Selecting items in the Notification Dialog will highlight the attached entity

### Model:
* Implemented Enumerations (#126)
  * Enum entities are modelled in the Interface Definition Aspect
    * EnumMembers are added to an Enum to represent the different values
  * EnumInstance entities can be used in Aggregates
  * EnumInstances in a ComponentBehaviour can be connected to any EnumMember contained in then EnumInstances Definition
* Header entities now uses a code data tag
  * Code will be directly injected into the top of the Impl.cpp file verbatim. This allows the definition of functions and header includes to be used. (#112)
* ComponentImpl is now automatically constructed when the user constructs a Component in the Interface Aspect (#122)
   * Loading in a  model containing only a defined Component, with no attached ComponentImpl will not auto-create an ComponentImpl. This functionality is only enabled via the dock/context toolbar.

### Workers:
* Updated Utility Worker(#108, #109, #112, #119)
  * Added Functions: Sleep, USleep. Used for sleeping the current thread for a period of time (Sleep=seconds, USleep=microseconds)
  * Added Functions: GetRandomUniformInt, GetRandomUniformReal, GetRandomExponentialReal and GetRandomNormalReal functions. Used for getting random numbers (Implemented using std::default_random_engine)
  * Added Function SetRandomSeed. Used to set the seed used by the std:: default_random_engine (and the above random functions)
  * Added Function: GetUUID. Used to generate a universal unique identification number, subsequent calls to this function generate different ids.

### Settings:
* Added general setting "RE Configure path" for selecting the configure.sh/.bat file RE uses for setting up paths
* Added general setting "CMAKE Generator" for selecting the generator Local Deployment will use when compiling
* Added theme settings for all Notification Severities colours
* Added theme settings for setting MEDEA's icon sizes
* Added theme setting for setting MEDEA's font

### Code Generation:
* Bug fixes to do with Attributes

### Resolved GitHub Issues:
* #76, #94, #99, #104, #107, #105, #108, #109, #112, #116, #119, #121 , #122, #125, #126, #127, #128, #129, #130

### Supporting software versions
* **re v1.3.1** (https://github.com/cdit-ma/re/releases/tag/v1.3.1)
* **re_gen v1.3.1** (https://github.com/cdit-ma/re_gen/releases/tag/v1.3.1)
* **logan v1.3.1** (https://github.com/cdit-ma/logan/releases/tag/v1.3.1)

---

# MEDEA v2.2.0-RE
## New Features:
### General:
* Implemented autosave functionality (#76) 
  * Open projects will autosave periodically (60 seconds)
  * The Autosave file will be created in the same directory as the current project, and will be named XXXX.autosave.graphml
  * The autosave file will only be written if there are changes to the model since the last save/autosave.
  * If a project is opened, and there was an autosave file, MEDEA will prompt the user to choose either the autosave/original project
  * Upon saving a project, the autosave file will be deemed out of date, and will be removed.
  * If an autosave project is opened, the user is shown the save-as dialog to select where to save this file.
  * Notifications will be displayed when these actions occur
* **Center On** functionality ensures visibility (#84, #92)
  * When using any centering functionality, MEDEA will ensure that item is visible. It will also ensure visibility of the **View Aspect Dock Widget** that item is contained in.
* Hovering over notifications will highlight their related Entity on the canvas (If they have one)
* Notification Manager and Search Widgets now both use minimizable group boxes which allow selection of filters.
* Search results are now filterable via their View Aspect
* Background processes (Loading Jenkins Nodes, Validation, Jenkins Execution) now have a spinning icon and appropriate labels in the Notification Manager
* Fixed Combo Box style inside Data Table

### Model:
* Implemented a **comment** data key (#85)
  * For Component, Aggregate, ComponentInstance, ComponentAssembly and all Behaviour components. 
  * Code generation for components will include these comments in code-generation.
* Implemented a **medea_version** data key for the Model for future recognition of changes between versions

### Settings:
* Added icons to all settings in the Settings GUI
* Replaced old group boxes with new minimizable group boxes in the Settings GUI
* Removed old settings which weren't required
* Refactored settings.ini structure to better support future changes
* Added a setting **Save Window State on exit** to store window position, size and state of the MEDEA window.
* Added a setting **Save Dock Widgets State on exit** to store arrangement, size and state of all MEDEA ViewAspects Dock Widgets in the MEDEA window.

### Model Validation:
* (#97) Validation catches behavioural entities which require data, but don't have any
* Added checks for double quotes around string values set via the data table
* Added check for invalid Attribute labels

### Code Generation:
* Fixed the code generation of data linking an InEventPortImpl's Aggregate directly to an OutEventPortImpl's Aggregate (#86) 
* Code generation of **comment** data elements into C++ code generated for ComponentImpl for enhanced traceability (#85)

### Resolved GitHub Issues:
* #97
* #85 
* #92 
* #86
* #84 
* #76
* #74
* #82

### Supporting software versions
* **re v1.3.0** (https://github.com/cdit-ma/re/releases/tag/v1.3.0)
* **re_gen v1.3.0** (https://github.com/cdit-ma/re_gen/releases/tag/v1.3.0)
* **logan v1.3.0** (https://github.com/cdit-ma/logan/releases/tag/v1.3.0)