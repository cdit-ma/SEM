<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    xmlns:cmake="http://github.com/cdit-ma/cmake"
    exclude-result-prefixes="gml exsl xalan">

    <!-- Load in Functions -->
    <xsl:import href="general_functions.xsl"/>
    <xsl:import href="cpp_functions.xsl"/>
    <xsl:import href="cmake_functions.xsl"/>
    <xsl:import href="graphml_functions.xsl"/>
    <xsl:import href="cdit_functions.xsl"/> 
    <xsl:import href="proto_functions.xsl"/>
    <xsl:import href="idl_functions.xsl"/>
    <xsl:import href="cdit_cmake_functions.xsl"/>
    
    <xsl:import href="datatype_convert.xsl"/>

    <!-- Middleware Input Parameter-->
    <xsl:param name="middlewares" as="xs:string" select="'Base'" />
    

    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
	 
    <xsl:template match="/">
        <xsl:variable name="parsed_middlewares" select="cdit:parse_middlewares($middlewares)" as="xs:string*" />
        <xsl:variable name="output_directory" select="'datatypes2'" as="xs:string*" />

        
        
        <xsl:for-each select="graphml:get_descendant_nodes_of_kind(., 'Aggregate')">
            <xsl:variable name="aggregate" select="." />
            <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
            <xsl:variable name="file_label" select="lower-case($aggregate_label)" />

            <xsl:value-of select="o:message(('Generating Datatype:', o:wrap_quote($aggregate_label)))" />

            <xsl:for-each select="$parsed_middlewares">
                <xsl:variable name="middleware" select="lower-case(.)" />
                <xsl:value-of select="o:message(('Generating Datatype:', o:wrap_quote($aggregate_label), 'For:', o:wrap_quote($middleware)))" />
                
                <xsl:variable name="aggregate_path" select="o:join_paths(($output_directory, $middleware, cdit:get_aggregates_path($aggregate)))" />
        
                <xsl:variable name="proto_file" select="concat($file_label, '.proto')" />
                <xsl:variable name="idl_file" select="concat($file_label, '.idl')" />
                <xsl:variable name="base_h" select="concat($file_label, '.h')" />
                <xsl:variable name="base_cpp" select="concat($file_label, '.cpp')" />
                <xsl:variable name="base_cmake" select="'CMakeLists.txt'" />

                <xsl:if test="cdit:build_shared_library($middleware)">
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'convert.h'))}">
                        <xsl:value-of select="cdit:get_convert_h($aggregate, $middleware)" />
                    </xsl:result-document>
                    
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'convert.cpp'))}">
                        <xsl:value-of select="cdit:get_convert_cpp($aggregate, $middleware)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_convert_cmake($aggregate, $middleware)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($aggregate_path, $proto_file))}">
                        <xsl:value-of select="cdit:get_proto_file($aggregate)" />
                    </xsl:result-document>

                    <xsl:if test="cdit:middleware_requires_idl_file($middleware)">
                        <xsl:result-document href="{o:write_file(($aggregate_path, $idl_file))}">
                            <xsl:value-of select="cdit:get_idl_file($aggregate)" />
                        </xsl:result-document>
                    </xsl:if>
                </xsl:if>

                <xsl:if test="cdit:build_module_library($middleware)">
                    <xsl:result-document href="{o:write_file(($aggregate_path, 'libportexports.cpp'))}">
                        <xsl:value-of select="cdit:get_port_export($aggregate, $middleware)" />
                    </xsl:result-document>
                </xsl:if>

                

                <xsl:if test="$middleware = 'base'">
                    <xsl:result-document href="{o:write_file(($aggregate_path, $base_h))}">
                        <xsl:value-of select="cdit:get_aggregate_base_h($aggregate)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($aggregate_path, $base_cpp))}">
                        <xsl:value-of select="cdit:get_aggregate_base_cpp($aggregate)" />
                    </xsl:result-document>

                    <xsl:result-document href="{o:write_file(($aggregate_path, cmake:cmake_file()))}">
                        <xsl:value-of select="cdit:get_aggregate_base_cmake($aggregate)" />
                    </xsl:result-document>
                </xsl:if>
            </xsl:for-each>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
