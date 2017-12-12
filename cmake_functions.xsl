<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:cmake="http://github.com/cdit-ma/cmake"
    >

    <!--
        Places the text (handles newlines) in a cmake comment, can be tabbed
    -->
    <xsl:function name="cmake:comment" as="xs:string">
        <xsl:param name="text" as="xs:string*" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="joined_text" select="o:join_list($text, ' ')" />

        <xsl:for-each select="tokenize($joined_text, '\n\r?')[.]">
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
        target_compile_definitions(${target} PRIVATE ${args})
    -->
    <xsl:function name="cmake:target_compile_definitions" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_compile_definitions', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), 'PRIVATE', o:wrap_dblquote($args)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Used to set a compiler flag for a particular target
        target_include_directories(${target} PRIVATE ${directory})
    -->
    <xsl:function name="cmake:target_include_directories" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="directory" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_include_directories', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), 'PRIVATE', o:wrap_dblquote($directory)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Used to link a library to a particular target
        target_link_libraries(${target} ${library_name})
    -->
    <xsl:function name="cmake:target_link_libraries" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_link_libraries', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), o:wrap_dblquote($library_name)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Used to include a subdirectory
    -->
    <xsl:function name="cmake:add_subdirectory">
        <xsl:param name="directory" as="xs:string" />
        <xsl:value-of select="concat('add_subdirectory(', o:wrap_dblquote($directory), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a find_library line

        # Find library re_core
        find_library(${libary_variable} ${library_name} "${library_path}")
    -->
    <xsl:function name="cmake:find_library">
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
    <xsl:function name="cmake:find_package">
        <xsl:param name="package_name" as="xs:string" />
        <xsl:param name="args" as="xs:string" />

        <xsl:value-of select="cmake:comment(concat('Find package ', $package_name), 0)" />
        <xsl:value-of select="concat('find_package', o:wrap_bracket(o:join_list(($package_name, $args), ' ')))" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Used to define variables in cmake

        set(${variable} ${value})
    -->
    <xsl:function name="cmake:set_variable" as="xs:string">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        

        <xsl:value-of select="concat(o:t($tab), 'set(', $variable, ' ', o:wrap_dblquote($value), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Used to define the project name in cmake

        set(PROJ_NAME ${project_name})
        project(${project_name)
    -->
    <xsl:function name="cmake:set_project_name">
        <xsl:param name="project_name" as="xs:string" />

        <xsl:value-of select="cmake:set_variable('PROJ_NAME', $project_name, 0)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl(1))" />
    </xsl:function>

    <!--
        Used to build a shared library

        add_library(${library_name} SHARED ${args})
    -->
    <xsl:function name="cmake:add_shared_library" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="library_type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />

        <xsl:value-of select="concat('add_library', o:wrap_bracket(o:join_list((cmake:wrap_variable($library_name), $library_type, $args), ' ')), o:nl(1))" />
    </xsl:function>


    <xsl:function name="cmake:if_start">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'if', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:if_elseif">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'elseif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:if_else">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'else', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:if_end">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'endif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:message">
        <xsl:param name="message" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'message', o:wrap_bracket(o:join_list(('STATUS', $message), ' ')), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:return">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'return', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:configure_file">
        <xsl:param name="input_file" as="xs:string"/>
        <xsl:param name="output_dir" as="xs:string"/>
        <xsl:value-of select="concat('configure_file', o:wrap_bracket(o:join_list(($input_file, $output_dir, 'COPYONLY'), ' ')), o:nl(1))" />
    </xsl:function>

     <xsl:function name="cmake:current_binary_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_BINARY_DIR')" />
    </xsl:function>

    <xsl:function name="cmake:current_source_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_SOURCE_DIR')" />
    </xsl:function>

    <xsl:function name="cmake:cmake_file" as="xs:string">
        <xsl:value-of select="'CMakeLists.txt'" />
    </xsl:function>

    <xsl:function name="cmake:add_subdirectories">
        <xsl:param name="sub_directories" as="xs:string*"/>

        <xsl:for-each select="$sub_directories">
            <xsl:value-of select="cmake:add_subdirectory(o:join_paths((cmake:current_source_dir_var(), .)))" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cmake:cmake_minimum_required">
        <xsl:param name="version" as="xs:string"/>
        <xsl:value-of select="concat('cmake_minimum_required(VERSION ', $version, ')', o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:set_cpp11">
        <xsl:value-of select="cmake:comment('CMake C++11 Options', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD', '11', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD_REQUIRED', 'ON', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_EXTENSIONS', 'OFF', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cmake:comment('MacOSX specific code', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_POSITION_INDEPENDENT_CODE', 'ON', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_BUILD_WITH_INSTALL_RPATH', 'ON', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_INSTALL_RPATH', concat('@loader_path;', o:and(), 'ORIGIN'), 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cmake:set_output_directory">
        <xsl:param name="var" as="xs:string"/>
        <xsl:param name="output_directory" as="xs:string"/>
        
        <xsl:value-of select="cmake:set_variable($var, $output_directory, 0)" />
        <xsl:value-of select="cmake:if_start('MSVC', 0)" />
        <xsl:value-of select="cmake:set_variable(concat($var, '_DEBUG'), cmake:wrap_variable($var), 1)" />
        <xsl:value-of select="cmake:set_variable(concat($var, '_RELEASE'), cmake:wrap_variable($var), 1)" />
        <xsl:value-of select="cmake:if_end('MSVC', 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cmake:set_runtime_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:value-of select="cmake:set_output_directory('CMAKE_RUNTIME_OUTPUT_DIRECTORY', $output_directory)" />
    </xsl:function>

    <xsl:function name="cmake:set_library_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:value-of select="cmake:set_output_directory('CMAKE_LIBRARY_OUTPUT_DIRECTORY', $output_directory)" />
    </xsl:function>

    <xsl:function name="cmake:set_archive_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:value-of select="cmake:set_output_directory('CMAKE_ARCHIVE_OUTPUT_DIRECTORY', $output_directory)" />
    </xsl:function>

    <!--
        gets the system environment variable in cmake syntax for access
        $env{variable_name}
    -->
    <xsl:function name="cmake:get_env_var" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$ENV', o:wrap_curly($variable_name))" />
    </xsl:function>


    <xsl:function name="cmake:setup_re_path">
        <xsl:value-of select="cmake:comment('CDIT Runtime Paths', 0)" />
        <xsl:value-of select="cmake:set_variable('RE_PATH', cmake:get_env_var('RE_PATH'), 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_MODULE_PATH', o:join_paths((cmake:wrap_variable('RE_PATH'), 'cmake_modules')), 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>




</xsl:stylesheet>