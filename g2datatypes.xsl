<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:cdit="http://github.com/cdit-ma"
    xmlns:o="http://github.com/cdit-ma"
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
        <xsl:variable name="middleware_path" select="'datatypes/'" />

        <xsl:for-each select="$aggregates">
            <!-- Get the namespace for the Aggregate (camel-cased idl_name) ,o:camel_case($idl_name) -->
            <xsl:variable name="base_namespace" select="cdit:get_namespace(.)" />

            <xsl:variable name="aggregate" as="element()*" select="." />

            <!-- Get the Aggregate's child Member, VectorInstance, AggregateInstance -->
            <xsl:variable name="members" as="element()*" select="cdit:get_child_entities_of_kind(., 'Member')" />
            <xsl:variable name="vector_def" as="element()*" select="cdit:get_child_entities_of_kind(., 'Vector')" />
            <xsl:variable name="vector_inst" as="element()*" select="cdit:get_child_entities_of_kind(., 'VectorInstance')" />
            <xsl:variable name="vectors" as="element()*" select="$vector_def, $vector_inst" />
            <xsl:variable name="aggregate_inst" as="element()*" select="cdit:get_child_entities_of_kind(., 'AggregateInstance')" />

            <!-- Get the label of the Aggregate -->
            <xsl:variable name="aggregate_label" select="lower-case(cdit:get_key_value(., 'label'))" />
            <xsl:variable name="aggregate_namespace" select="cdit:get_key_value(., 'namespace')" />
            <xsl:variable name="aggregate_namespace_lc" select="lower-case($aggregate_namespace)" />
            <xsl:variable name="aggregate_type" select="cdit:get_key_value(.,'type')" />

            <!-- Set the Middleware for this XSL -->
            <xsl:variable name="base_mw" select="'base'" />

            <!-- Setup the base type and middleware type-->            
            <!-- The middleware specific class is specified from the Model -->
            <xsl:variable name="mw_type" select="$aggregate_type" />
            <!-- The Base class is the mw_type with Base:: prepended-->
            <xsl:variable name="base_type" select="concat( 'Base::', $mw_type)" />

            <!-- Parse each Middleware -->
            <xsl:for-each select="tokenize(normalize-space($middlewares), ',')"> 
                <xsl:variable name="mw" select="." />
                <xsl:message>Parsing Middleware: <xsl:value-of select="$mw" /> </xsl:message>

                <xsl:variable name="middleware_namespace" select="$aggregate_namespace" />

                <xsl:variable name="current_mw_path" select="concat($middleware_path, $mw, '/')" />

                <!--Check whether this middleware builds a module/shared library-->
                <xsl:variable name="builds_module" select="o:cmake_mw_builds_module($mw) = true()" />
                <xsl:variable name="builds_library" select="o:cmake_mw_builds_shared_library($mw) = true()" />
                
                <xsl:variable name="current_aggregate_path" select="concat($current_mw_path, $aggregate_namespace_lc, '/', $aggregate_label, '/')" />
                
                <!-- Define output files -->
                <xsl:variable name="port_proto" select="concat($current_aggregate_path, $aggregate_label, '.proto')" />
                <xsl:variable name="port_idl" select="concat($current_aggregate_path, $aggregate_label, '.idl')" />
                <xsl:variable name="port_convert_h" select="concat($current_aggregate_path, 'convert.h')" />
                <xsl:variable name="port_convert_cpp" select="concat($current_aggregate_path, 'convert.cpp')" />
                <xsl:variable name="port_libexport_cpp" select="concat($current_aggregate_path, 'libportexports.cpp')" />
                <xsl:variable name="port_cmake" select="concat($current_aggregate_path, 'CMakeLists.txt')" />

                
                <!-- Protobuf requires a .proto file-->
                <xsl:if test="$mw = 'proto'">
                    <xsl:result-document href="{o:xsl_wrap_file($port_proto)}">
                        <xsl:value-of select="o:get_proto($aggregate, $members, $vectors, $aggregate_inst)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- DDS Implementations require a .idl file-->
                <xsl:if test="$mw = 'rti' or $mw = 'ospl'">
                    <xsl:result-document href="{o:xsl_wrap_file($port_idl)}">
                        <xsl:value-of select="o:get_idl($aggregate, $members, $vectors, $aggregate_inst)" />
                    </xsl:result-document>
                </xsl:if>
            
                <!-- If building a shared library, we required a convert class to translate between the middleware specific class and the base class-->
                <xsl:if test="$builds_library">
                    <xsl:result-document href="{o:xsl_wrap_file($port_convert_h)}">
                        <xsl:value-of select="o:get_convert_h($aggregate, $mw_type, $base_type, $mw, $middleware_namespace)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:xsl_wrap_file($port_convert_cpp)}">
                        <xsl:value-of select="o:get_convert_cpp($aggregate, $members, $vectors, $aggregate_inst, $mw_type, $base_type, $mw, $base_mw, $middleware_namespace)" />
                    </xsl:result-document>
                </xsl:if>

                <!-- If building a module, we required an interface class so that ports can be constructed via a dynamic loading of the module-->
                <xsl:if test="$builds_module">
                    <xsl:result-document href="{o:xsl_wrap_file($port_libexport_cpp)}">
                        <xsl:value-of select="o:get_libport_export($aggregate, $mw, $base_type, $mw_type)" />
                    </xsl:result-document>
                </xsl:if>
            </xsl:for-each>

        <xsl:variable name="base_path" select="concat($middleware_path, 'base/')" />
        <xsl:variable name="base_aggregate_path" select="concat($base_path, $aggregate_namespace_lc, '/', $aggregate_label, '/')" />

        <xsl:variable name="ports_cmake" select="concat($middleware_path, 'CMakeLists.txt')" />
        
        <xsl:variable name="base_type_cpp" select="concat($base_aggregate_path, $aggregate_label, '.cpp')" />
        <xsl:variable name="base_type_h" select="concat($base_aggregate_path, $aggregate_label, '.h')" />
        <xsl:variable name="base_type_cmake" select="concat($base_aggregate_path, 'CMakeLists.txt')" />

        <!-- Write base type class -->
        <xsl:result-document href="{o:xsl_wrap_file($base_type_h)}">
            <xsl:value-of select="o:get_base_data_type_h($aggregate, $members, $vectors, $aggregate_inst)" />
        </xsl:result-document>

        <xsl:result-document href="{o:xsl_wrap_file($base_type_cpp)}">
            <xsl:value-of select="o:get_base_data_type_cpp($aggregate, $members, $vectors, $aggregate_inst)" />
        </xsl:result-document>

        <xsl:result-document href="{o:xsl_wrap_file($base_type_cmake)}">
            <xsl:value-of select="o:get_base_data_type_cmake($aggregate, $members, $vectors, $aggregate_inst)" />
        </xsl:result-document>


        </xsl:for-each>

        <xsl:variable name="datatype_cmake" select="'datatypes/CMakeLists.txt'" />
        <!-- Write out a datatype cmake file -->
        <xsl:variable name="middleware_list" select="tokenize(normalize-space(concat($middlewares, ',base')), ',')" />
        <xsl:result-document href="{o:xsl_wrap_file($datatype_cmake)}">
            <xsl:value-of select="cdit:get_middleware_subfolder_cmake($middleware_list, $aggregates)" />
        </xsl:result-document>

        <xsl:variable name="top_cmake" select="'CMakeLists.txt'" />

        <xsl:result-document href="{o:xsl_wrap_file($top_cmake)}">
            <xsl:value-of select="cdit:get_top_level_cmake_cmake()" />
        </xsl:result-document>

    </xsl:template>
</xsl:stylesheet>