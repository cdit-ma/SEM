<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    exclude-result-prefixes="gml exsl xalan">
		
    <!-- Load in Functions -->
    <xsl:import href="functions.xsl"/>
    
    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />

    <!-- Middleware Input Parameter-->
    <xsl:param name="middlewares"></xsl:param>
    
    <!-- Get all of the Aggregates -->
    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(., 'Aggregate')" />

    <xsl:template match="/">
        <xsl:for-each select="tokenize(normalize-space($middlewares), ',')"> 
            <xsl:variable name="middleware" select="." />
            <xsl:message>Parsing Middleware: <xsl:value-of select="$middleware" /> </xsl:message>
            <xsl:for-each select="$aggregates">
                <!-- Get the name of the IDL this aggregate belongs to -->
                <xsl:variable name="idl_name" select="cdit:get_key_value(../..,'label')" />

                <!-- Get the namespace for the Aggregate (camel-cased idl_name) -->
                <xsl:variable name="namespace" select="o:camel_case($idl_name)" />

                <!-- Get the Aggregate's child Member, VectorInstance, AggregateInstance -->
                <xsl:variable name="members" as="element()*" select="cdit:get_child_entities_of_kind(., 'Member')" />
                <xsl:variable name="vectors" as="element()*" select="cdit:get_child_entities_of_kind(., 'VectorInstance')" />
                <xsl:variable name="aggregates" as="element()*" select="cdit:get_child_entities_of_kind(., 'AggregateInstance')" />

                <!-- Get the label of the Aggregate -->
                <xsl:variable name="aggregate_label" select="cdit:get_key_value(.,'label')" />
                <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
                <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

                <!-- Set the Middleware for this XSL -->
                <xsl:variable name="mw" select="$middleware" />
                <xsl:variable name="base_mw" select="'base'" />

                <!-- Setup the base type and middleware type-->            
                <xsl:variable name="base_type" select="concat('::', $aggregate_label_cc)" />
                <xsl:variable name="mw_type" select="concat($namespace, '::', $aggregate_label_cc)" />

                <!--Check whether this middleware builds a module/shared library-->
                <xsl:variable name="builds_module" select="o:cmake_mw_builds_module($mw) = true()" />
                <xsl:variable name="builds_library" select="o:cmake_mw_builds_shared_library($mw) = true()" />
                
                <!-- Protobuf requires a .proto file-->
                <xsl:if test="$mw = 'proto'">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.proto -->
                    <xsl:variable name="proto_file" select="concat('middleware/', $mw, '/', $aggregate_label_lc, '/', $aggregate_label_lc, '.proto')" />
                
                    <xsl:result-document href="{o:xsl_wrap_file($proto_file)}">
                        <xsl:value-of select="o:get_proto(., $members, $vectors, $aggregates)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- DDS Implementations require a .idl file-->
                <xsl:if test="$mw = 'rti' or $mw = 'ospl'">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.idl -->
                    <xsl:variable name="idl_file" select="concat('middleware/', $mw, '/', $aggregate_label_lc, '/', $aggregate_label_lc, '.idl')" />
                
                    <xsl:result-document href="{o:xsl_wrap_file($idl_file)}">
                        <xsl:value-of select="o:get_idl(., $members, $vectors, $aggregates)" />
                    </xsl:result-document>
                </xsl:if>
                
                <!-- If building a shared library, we required a convert class to translate between the middleware specific class and the base class-->
                <xsl:if test="$builds_library">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/convert.h -->
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/convert.cpp -->
                    <xsl:variable name="convert_cpp_file" select="concat('middleware/', $mw, '/', $aggregate_label_lc, '/', 'convert.cpp')" />
                    <xsl:variable name="convert_h_file" select="concat('middleware/', $mw, '/', $aggregate_label_lc, '/', 'convert.h')" />
                    
                    <xsl:result-document href="{o:xsl_wrap_file($convert_h_file)}">
                        <xsl:value-of select="o:get_convert_h(., $mw_type, $base_type, $mw, $mw)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:xsl_wrap_file($convert_cpp_file)}">
                        <xsl:value-of select="o:get_convert_cpp(., $members, $vectors, $aggregates, $mw_type, $base_type, $mw, $base_mw, $mw)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- If building a module, we required an interface class so that ports can be constructed via a dynamic loading of the module-->
                <xsl:if test="$builds_module">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/libportexports.cpp -->
                    <xsl:variable name="libportexport_cpp_file" select="concat('middleware/', $mw, '/', $aggregate_label_lc, '/', 'libportexports.cpp')" />

                    <xsl:result-document href="{o:xsl_wrap_file($libportexport_cpp_file)}">
                        <xsl:value-of select="o:get_libport_export(., $mw, $base_type, $mw_type)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- Using CMake for make file construction-->
                <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/CMakeLists.txt -->
                <xsl:result-document href="{concat('middleware/', $mw, '/', $aggregate_label_lc, '/CMakeLists.txt')}">
                    <xsl:value-of select="o:get_mw_type_cmake(., $members, $vectors, $aggregates, $mw)" />
                </xsl:result-document>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>