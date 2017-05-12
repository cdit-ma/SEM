# re_gen
re_gen is a set of XSL transforms to perform code generation on a MEDEA graphml model. They are written in XSL2.0 and thus require a XSLv2 compliant processor ie. Saxon.

## Required Software:
* Saxon
* Java

## Generating Component code
* Will generate into $$PWD/components
* ``java -jar saxon.jar -xsl:g2components.xsl -s:model.graphml``

## Generating Datatypes code
* Will generate into $$PWD/datatypes
* java -jar saxon.jar -xsl:g2components.xsl -s:model.graphml middlewares=rti,ospl,zmq,qpid

## Generating DDS QoS Artifacts
* Will generate files into $$PWD/qos
* ``java -jar saxon.jar -xsl:g2validate.xsl -s:model.graphml``

## Validating Model
* Will produce a report in std output
* ``java -jar saxon.jar -xsl:g2validate.xsl -s:model.graphml``

