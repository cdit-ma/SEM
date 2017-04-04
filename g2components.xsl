<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    exclude-result-prefixes="gml exsl xalan">

    <!-- Load in Functions -->
    <xsl:import href="component_functions.xsl"/>
    
    <xsl:output method="text" omit-xml-declaration="yes" indent="yes" standalone="no" />
	
    <!-- Get all of the Aggregates -->
    <xsl:variable name="components" as="element()*" select="cdit:get_entities_of_kind(., 'ComponentImpl')" />

    <xsl:template match="/">
        <!-- Construct file paths -->
        <xsl:variable name="component_path" select="'components/'" />
        
        <xsl:for-each select="$components">
            <xsl:variable name="component_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="component_label_lc" select="lower-case($component_label)" />

            <xsl:variable name="component_base_path" select="concat($component_path, $component_label_lc, '/')" />

            <xsl:variable name="component_int_h" select="concat($component_base_path, $component_label_lc, 'int.h')" />
            <xsl:variable name="component_int_cpp" select="concat($component_base_path, $component_label_lc, 'int.cpp')" />
            <xsl:variable name="component_impl_h" select="concat($component_base_path, $component_label_lc, 'impl.h')" />
            <xsl:variable name="component_impl_cpp" select="concat($component_base_path, $component_label_lc, 'impl.cpp')" />
            <xsl:variable name="component_export_cpp" select="concat($component_base_path, 'libcomponentexports.cpp')" />
            <xsl:variable name="component_cmake" select="concat($component_base_path, 'CMakeLists.txt')" />
            
            
            <!-- Write File: components/{COMPONENT_LABEL}/{COMPONENT_LABEL}int.h -->
            <xsl:result-document href="{o:xsl_wrap_file($component_int_h)}">
                <xsl:value-of select="cdit:get_component_int_h(.)" />
            </xsl:result-document>

            <!-- Write File: components/{COMPONENT_LABEL}/{COMPONENT_LABEL}int.cpp -->
            <xsl:result-document href="{o:xsl_wrap_file($component_int_cpp)}">
                <xsl:value-of select="cdit:get_component_int_cpp(.)" />
            </xsl:result-document>

            <!-- Write File: components/{COMPONENT_LABEL}/{COMPONENT_LABEL}impl.h -->
            <xsl:result-document href="{o:xsl_wrap_file($component_impl_h)}">
                <xsl:value-of select="cdit:get_component_impl_h(.)" />
            </xsl:result-document>
            
            <!-- Write File: components/{COMPONENT_LABEL}/{COMPONENT_LABEL}impl.cpp -->
            <xsl:result-document href="{o:xsl_wrap_file($component_impl_cpp)}">
                <xsl:value-of select="cdit:get_component_impl_cpp(.)" />
            </xsl:result-document>

            <!-- Write File: components/{COMPONENT_LABEL}/libcomponentexports.cpp -->
            <xsl:result-document href="{o:xsl_wrap_file($component_export_cpp)}">
                <xsl:value-of select="cdit:get_libcomponent_export_cpp(.)" />
            </xsl:result-document>

            <!-- Write File: components/{COMPONENT_LABEL}/CMakeLists.txt -->
            <xsl:result-document href="{o:xsl_wrap_file($component_cmake)}">
                <xsl:value-of select="cdit:get_component_cmake(.)" />
            </xsl:result-document>
        </xsl:for-each>

        <xsl:variable name="components_cmake" select="concat($component_path, 'CMakeLists.txt')" />
        <!-- Write File: components/CMakeLists.txt -->
        <xsl:result-document href="{o:xsl_wrap_file($components_cmake)}">
            <xsl:value-of select="cdit:get_component_folder_cmake($components)" />
        </xsl:result-document>
    </xsl:template>
</xsl:stylesheet>
