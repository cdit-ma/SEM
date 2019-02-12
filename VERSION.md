# MEDEA v3.3.3
## New Features:
* Allowed the labels/index of all parameters contained in a InputParameterGroup/ReturnParameterGroup to be editable
* Added a new human-readable flag ('r') mode to medea_cli which exports the Models Data keys using the key_name as it's ID to help improve readability of the exported mode.

## Bug Fixes:
* Fixed an issue with for-loops not passing validation. The default type of lhs for both Itterator/Comparator weren't previously set (Changed to Integer), which meant that code-gen flagged a type mismatch.
* Fixed an issue with Setter entities not loading connected edges when using the production operators (*,+,-,/)
* When loading a model which constructs an implicitly constructed entity (ForLoop etc), Any <data> element which attempts to clear the implicitly set data (with a blank string/value) will be ignored from loading.
* Fixed an issue where theme colours would default to black


## Resolved JIRA Tickets:
* MED-486
* MED-496
* MED-506
* MED-508
* REGEN-92
