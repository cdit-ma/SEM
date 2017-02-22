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
        <!-- Construct file paths -->
        <xsl:variable name="middleware_path" select="'middleware/'" />

        <xsl:for-each select="tokenize(normalize-space($middlewares), ',')"> 
            <xsl:variable name="middleware" select="." />

            <xsl:variable name="current_mw_path" select="concat($middleware_path, $middleware, '/')" />


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

                <xsl:variable name="current_aggregate_path" select="concat($current_mw_path, $aggregate_label_lc, '/')" />

                

                <!-- Set the Middleware for this XSL -->
                <xsl:variable name="mw" select="$middleware" />
                <xsl:variable name="base_mw" select="'base'" />

                <!-- Setup the base type and middleware type-->            
                <xsl:variable name="base_type" select="concat('::', $aggregate_label_cc)" />
                <xsl:variable name="mw_type" select="concat($namespace, '::', $aggregate_label_cc)" />

                <!--Check whether this middleware builds a module/shared library-->
                <xsl:variable name="builds_module" select="o:cmake_mw_builds_module($mw) = true()" />
                <xsl:variable name="builds_library" select="o:cmake_mw_builds_shared_library($mw) = true()" />
                

                <xsl:variable name="port_proto" select="concat($current_aggregate_path, $aggregate_label_lc, '.proto')" />
                <xsl:variable name="port_idl" select="concat($current_aggregate_path, $aggregate_label_lc, '.idl')" />
                <xsl:variable name="port_convert_h" select="concat($current_aggregate_path, 'convert.h')" />
                <xsl:variable name="port_convert_cpp" select="concat($current_aggregate_path, 'convert.cpp')" />
                <xsl:variable name="port_libexport_cpp" select="concat($current_aggregate_path, 'libportexports.cpp')" />
                <xsl:variable name="port_cmake" select="concat($current_aggregate_path, 'CMakeLists.txt')" />

                    
                <!-- Protobuf requires a .proto file-->
                <xsl:if test="$mw = 'proto'">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.proto -->
                    <xsl:result-document href="{o:xsl_wrap_file($port_proto)}">
                        <xsl:value-of select="o:get_proto(., $members, $vectors, $aggregates)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- DDS Implementations require a .idl file-->
                <xsl:if test="$mw = 'rti' or $mw = 'ospl'">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/{AGGREGATE_LABEL}.idl -->
                    <xsl:result-document href="{o:xsl_wrap_file($port_idl)}">
                        <xsl:value-of select="o:get_idl(., $members, $vectors, $aggregates)" />
                    </xsl:result-document>
                </xsl:if>
                
                <!-- If building a shared library, we required a convert class to translate between the middleware specific class and the base class-->
                <xsl:if test="$builds_library">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/convert.h -->
                    <xsl:result-document href="{o:xsl_wrap_file($port_convert_h)}">
                        <xsl:value-of select="o:get_convert_h(., $mw_type, $base_type, $mw, $mw)" />
                    </xsl:result-document>

                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/convert.cpp -->
                    <xsl:result-document href="{o:xsl_wrap_file($port_convert_cpp)}">
                        <xsl:value-of select="o:get_convert_cpp(., $members, $vectors, $aggregates, $mw_type, $base_type, $mw, $base_mw, $mw)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- If building a module, we required an interface class so that ports can be constructed via a dynamic loading of the module-->
                <xsl:if test="$builds_module">
                    <!-- Write File: middleware/{MIDDLEWARE}/{AGGREGATE_LABEL}/libportexports.cpp -->
                    <xsl:result-document href="{o:xsl_wrap_file($port_libexport_cpp)}">
                        <xsl:value-of select="o:get_libport_export(., $mw, $base_type, $mw_type)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- Using CMake for make file construction-->
                <!-- Write File: middleware/{MIDDLEWARE}/CMakeLists.txt -->
                <xsl:result-document href="{o:xsl_wrap_file($port_cmake)}">
                    <xsl:value-of select="o:get_mw_type_cmake(., $members, $vectors, $aggregates, $mw)" />
                </xsl:result-document>
            </xsl:for-each>

            <xsl:variable name="ports_cmake" select="concat($current_mw_path, 'CMakeLists.txt')" />
            <!-- Write File: middleware/{MIDDLEWARE}/CMakeLists.txt -->
            <xsl:result-document href="{o:xsl_wrap_file($ports_cmake)}">
                <xsl:value-of select="cdit:get_subfolder_cmake($aggregates)" />
            </xsl:result-document>
        </xsl:for-each>
        <xsl:variable name="middleware_cmake" select="concat($middleware_path, 'CMakeLists.txt')" />
            <!-- Write File: middleware/CMakeLists.txt -->
            <xsl:result-document href="{o:xsl_wrap_file($middleware_cmake)}">
                <xsl:value-of select="cdit:get_subfolder_cmake_from_list($middlewares)" />
            </xsl:result-document>
    </xsl:template>
</xsl:stylesheet>