<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma"
    xmlns:cmake="http://github.com/cdit-ma"
    >

    <!--
        Places the text (handles newlines) in a cmake comment, can be tabbed
    -->
    <xsl:function name="cmake:comment" as="xs:string">
        <xsl:param name="text" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:for-each select="tokenize($text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '# ', ., o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Wraps the variable in cmake syntax for access
        ${variable_name}
    -->
    <xsl:function name="cmake:wrap_variable" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$', o:wrap_curly($variable_name))" />
    </xsl:function>

    <!--
        Used to include a directory for a particular target
        target_include_directories(${target} PRIVATE ${directory})
    -->
    <xsl:function name="cmake:target_include_directories" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="directory" as="xs:string" />
        <xsl:value-of select="concat('target_include_directories(', $target, ' PRIVATE ', o:wrap_dblquote($directory), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Used to link a library to a particular target
        target_link_libraries(${target} ${library_name})
    -->
    <xsl:function name="cmake:target_link_libraries" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:value-of select="concat('target_link_libraries(', $target, ' ', $library_name, ')', o:nl(1))" />
    </xsl:function>

    <!--
        Used to include a subdirectory
    -->
    <xsl:function name="cmake:add_subdirectory" as="xs:string">
        <xsl:param name="directory" as="xs:string" />
        <xsl:value-of select="concat('add_subdirectory(', o:wrap_dblquote($directory), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a find_library line

        # Find library re_core
        find_library(${libary_variable} ${library_name} "${library_path}")
    -->
    <xsl:function name="cmake:find_library" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="libary_variable" as="xs:string" />
        <xsl:param name="library_path" as="xs:string" />

        <xsl:value-of select="cmake:comment(concat('Find library ', $library_name), 0)" />
        <xsl:value-of select="concat('find_library(', $libary_variable, ' ', $library_name, ' ', o:wrap_dblquote($library_path), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a find_package line

        # Find library re_core
        find_library(${package_name} ${args})
    -->
    <xsl:function name="cmake:find_package" as="xs:string">
        <xsl:param name="package_name" as="xs:string" />
        <xsl:param name="args" as="xs:string" />

        <xsl:value-of select="cmake:comment(concat('Find package ', $package_name), 0)" />
        <xsl:value-of select="concat('find_package(', $package_name)" />
        <xsl:if test="$args != ''">
            <xsl:value-of select="concat(' ', $args)" />
        </xsl:if>
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Used to define variables in cmake

        set(${variable} ${value})
    -->
    <xsl:function name="cmake:set_variable" as="xs:string">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:value-of select="concat('set(', $variable, ' ', o:wrap_dblquote($value), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Used to define the project name in cmake

        set(PROJ_NAME ${project_name})
        project(${project_name)
    -->
    <xsl:function name="cmake:set_project_name" as="xs:string">
        <xsl:param name="project_name" as="xs:string" />

        <xsl:value-of select="cmake:set_variable('PROJ_NAME', $project_name)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl(1))" />
    </xsl:function>

    <!--
        Used to build a shared library

        add_library(${library_name} SHARED ${args})
    -->
    <xsl:function name="cmake:add_shared_library" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="sources" as="xs:string" />
        <xsl:param name="headers" as="xs:string" />

        <xsl:value-of select="concat('add_library(', $library_name, ' SHARED')" />

        <xsl:if test="$sources != ''">
            <xsl:value-of select="concat(' ', $sources)" />
        </xsl:if>

        <xsl:if test="$headers != ''">
            <xsl:value-of select="concat(' ', $headers)" />
        </xsl:if>

        <xsl:value-of select="concat(')', o:nl(1))" />
    </xsl:function>
</xsl:stylesheet>