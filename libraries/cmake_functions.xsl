<!--
    A set of XSLT2.0 Functions for outputting CMake code
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    >

    <!--
        Places the text (handles newlines) in a comment, can be tabbed
    -->
    <xsl:function name="cmake:comment">
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />

        <xsl:for-each select="tokenize($joined_text, '\n\r?')[.]">
            <xsl:value-of select="concat(o:t($tab), '# ', normalize-space(.), o:nl(1))" />
        </xsl:for-each>
    </xsl:function>

    <!--
        Wraps the string provided in cmake variable syntax
        ie. ${variable}
    -->
    <xsl:function name="cmake:wrap_variable" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$', o:wrap_curly($variable_name))" />
    </xsl:function>

    <!--
        Sets a compiler definition for a particular target (will wrap target using cmake:wrap_variable)
        ie. target_compile_definitions(${target} PRIVATE ${args})
    -->
    <xsl:function name="cmake:target_compile_definitions" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_compile_definitions', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), 'PRIVATE', o:wrap_dblquote($args)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Includes a directory for a particular target (will wrap target using cmake:wrap_variable)
        target_include_directories(${target} PRIVATE ${directory})
    -->
    <xsl:function name="cmake:target_include_directories" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="directory" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_include_directories', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), 'PRIVATE', o:wrap_dblquote($directory)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Links a library against a particular target (will wrap target using cmake:wrap_variable)
        target_link_libraries(${target} ${library_name})
    -->
    <xsl:function name="cmake:target_link_libraries" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_link_libraries', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), o:wrap_dblquote($library_name)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Adds a subdirectory
    -->
    <xsl:function name="cmake:add_subdirectory">
        <xsl:param name="directory" as="xs:string" />
        <xsl:value-of select="concat('add_subdirectory(', o:wrap_dblquote($directory), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Finds a library

        # Find library re_core
        find_library(${libary_variable} ${library_name} "${library_path}")
    -->
    <xsl:function name="cmake:find_library">
        <xsl:param name="libary_variable" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="library_path" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:comment(concat('Find library ', $library_name), $tab)" />
        <xsl:value-of select="concat(o:t($tab), 'find_library(', $libary_variable, ' ', $library_name, ' ', o:wrap_dblquote($library_path), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a find_package line

        # Find package re_core
        find_package(${package_name} ${args})
    -->
    <xsl:function name="cmake:find_package">
        <xsl:param name="package_name" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:comment(concat('Find package ', $package_name), $tab)" />
        <xsl:value-of select="concat(o:t($tab), 'find_package', o:wrap_bracket(o:join_list(($package_name, $args), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Sets a variable
        ie. set(${variable} ${value})
    -->
    <xsl:function name="cmake:set_variable" as="xs:string">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        

        <xsl:value-of select="concat(o:t($tab), 'set(', $variable, ' ', o:wrap_dblquote($value), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Sets a variable
        ie. set(${variable} ${value})
    -->
    <xsl:function name="cmake:set_variable_if_not_set" as="xs:string*">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:sequence select="cmake:if_start(concat('NOT ', $variable), $tab)" />
        <xsl:sequence select="cmake:set_variable($variable, $value, $tab + 1)" />
        <xsl:sequence select="cmake:if_end(concat('NOT ', $variable), $tab)" />
    </xsl:function>

    

    <!--
        Sets the project name

        ie. set(PROJ_NAME ${project_name})
        project(${project_name)
    -->
    <xsl:function name="cmake:set_project_name">
        <xsl:param name="project_name" as="xs:string" />

        <xsl:value-of select="cmake:set_variable('PROJ_NAME', $project_name, 0)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl(1))" />
    </xsl:function>

    <!--
        Adds a shared library (MODULE/SHARED)
        ie. add_library(${library_name} ${library_type} ${args})
    -->
    <xsl:function name="cmake:add_shared_library" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="library_type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />

        <xsl:value-of select="concat('add_library', o:wrap_bracket(o:join_list((cmake:wrap_variable($library_name), $library_type, $args), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an if statement
        ie. if(${condition})
    -->
    <xsl:function name="cmake:if_start">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'if', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an else if statement
        ie. elseif(${condition})
    -->
    <xsl:function name="cmake:if_elseif">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'elseif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an else statement
        ie. else()
    -->
    <xsl:function name="cmake:if_else">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'else', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <!--
        Ends an if statement
        ie. endif(${condition})
    -->
    <xsl:function name="cmake:if_end">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'endif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Prints a CMake message when processed
    -->
    <xsl:function name="cmake:message">
        <xsl:param name="message" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'message', o:wrap_bracket(o:join_list(('STATUS', $message), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Returns out of a CMake function
    -->
    <xsl:function name="cmake:return">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'return', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <!--
        Copys a file into a directory
    -->
    <xsl:function name="cmake:configure_file">
        <xsl:param name="input_file" as="xs:string"/>
        <xsl:param name="output_dir" as="xs:string"/>
        <xsl:value-of select="concat('configure_file', o:wrap_bracket(o:join_list(($input_file, $output_dir, 'COPYONLY'), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Gets the current binary directory variable
        ie. ${CMAKE_CURRENT_BINARY_DIR}
    -->
    <xsl:function name="cmake:current_binary_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_BINARY_DIR')" />
    </xsl:function>

    <!--
        Gets the current source directory variable
        ie. ${CMAKE_CURRENT_SOURCE_DIR}
    -->
    <xsl:function name="cmake:current_source_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_SOURCE_DIR')" />
    </xsl:function>

    <!--
        Gets CMakeLists.txt
    -->
    <xsl:function name="cmake:cmake_file" as="xs:string">
        <xsl:value-of select="'CMakeLists.txt'" />
    </xsl:function>

    <!--
        Adds a list of sub_directories, will prepend on the current source dir
        ie.
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${sub_directories}[1])
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${sub_directories}[2])
    -->
    <xsl:function name="cmake:add_subdirectories">
        <xsl:param name="sub_directories" as="xs:string*"/>

        <xsl:for-each select="$sub_directories">
            <xsl:if test=". != ''">
                <xsl:value-of select="cmake:add_subdirectory(o:join_paths((cmake:current_source_dir_var(), .)))" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Sets the minimum CMake version required
        ie. cmake_minimum_required(VERSION 3.1)
    -->
    <xsl:function name="cmake:cmake_minimum_required">
        <xsl:param name="version" as="xs:string"/>
        <xsl:value-of select="concat('cmake_minimum_required(VERSION ', $version, ')', o:nl(1))" />
    </xsl:function>

    <!--
        Sets the required settings to force C++11 Compliance
    -->
    <xsl:function name="cmake:set_cpp11">
        <xsl:value-of select="cmake:comment('CMake C++11 Options', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD', '11', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD_REQUIRED', 'ON', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_EXTENSIONS', 'OFF', 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Sets the output directory for ${var} to the provided ${output_directory}
    -->
    <xsl:function name="cmake:set_output_directory">
        <xsl:param name="var" as="xs:string"/>
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="cmake:set_variable($var, $output_directory, $tab)" />
        <xsl:value-of select="cmake:if_start('MSVC', $tab)" />
        <xsl:value-of select="cmake:set_variable(concat($var, '_DEBUG'), cmake:wrap_variable($var), $tab + 1)" />
        <xsl:value-of select="cmake:set_variable(concat($var, '_RELEASE'), cmake:wrap_variable($var), $tab + 1)" />
        <xsl:value-of select="cmake:if_end('MSVC', $tab)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Sets the runtime output directory to the provided ${output_directory}
    -->
    <xsl:function name="cmake:set_runtime_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_RUNTIME_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <!--
        Sets the library output directory to the provided ${output_directory}
    -->
    <xsl:function name="cmake:set_library_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_LIBRARY_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <!--
        Sets the archive output directory to the provided ${output_directory}
    -->
    <xsl:function name="cmake:set_archive_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_ARCHIVE_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <xsl:function name="cmake:set_property">
        <xsl:param name="property" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'set_property(', $property, ')', o:nl(1))" />
    </xsl:function>

    <!--
        Gets a system environment variable
        ie. $env{variable_name}
    -->
    <xsl:function name="cmake:get_env_var" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$ENV', o:wrap_curly($variable_name))" />
    </xsl:function>

    <!--
        Sets a project to use ccache if available
    -->
    <xsl:function name="cmake:use_ccache">
        <xsl:param name="tab" as="xs:integer" />
        
        <xsl:value-of select="concat(o:t($tab), 'find_program(CCACHE_FOUND ccache)', o:nl(1))" />
        <xsl:value-of select="cmake:if_start('CCACHE_FOUND', $tab)" />
        <xsl:value-of select="cmake:set_property('GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache', 1)" />
        <xsl:value-of select="cmake:set_property('GLOBAL PROPERTY RULE_LAUNCH_LINK ccache', 1)" />
        <xsl:value-of select="cmake:if_end('CCACHE_FOUND', $tab)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

</xsl:stylesheet>