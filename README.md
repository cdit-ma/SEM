# re_gen
re_gen is a set of XSL transforms to perform code generation on a MEDEA graphml model. They are written in XSL2.0 and thus require a XSLv2 compliant processor ie. Saxon.

## Required Software:
* Saxon
* Java

## Generating Project code
* Will generate a CMake compiler project/folder structure in the PWD
* ``java -jar saxon.jar -xsl:generate_project.xsl -s:model.graphml``

## Generating Validation report
* Will produce a report in std out
* ``java -jar saxon.jar -xsl:generate_validation.xsl -s:model.graphml``

