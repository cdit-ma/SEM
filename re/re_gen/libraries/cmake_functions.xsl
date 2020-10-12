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
        # ${test}
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
        ${variable_name}
    -->
    <xsl:function name="cmake:wrap_variable" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$', o:wrap_curly($variable_name))" />
    </xsl:function>

    <!--
        Sets a compiler definition for a particular target (will wrap target using cmake:wrap_variable)
        target_compile_definitions(${target} ${scope} "${args}")
    -->
    <xsl:function name="cmake:target_compile_definitions" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="scope" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_compile_definitions', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), $scope, o:wrap_dblquote($args)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Includes a directory for a particular target (will wrap target using cmake:wrap_variable)
        target_include_directories(${target} ${scope} "${directory}")
    -->
    <xsl:function name="cmake:target_include_directories" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="scope" as="xs:string" />
        <xsl:param name="directory" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_include_directories', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), $scope, o:wrap_dblquote($directory)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Links a library against a particular target (will wrap target using cmake:wrap_variable)
        target_link_libraries(${target} ${scope} ${library_name})
    -->
    <xsl:function name="cmake:target_link_libraries" as="xs:string">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="scope" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'target_link_libraries', o:wrap_bracket(o:join_list((cmake:wrap_variable($target), $scope, o:wrap_dblquote($library_name)), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Adds a subdirectory
        add_subdirectory("${directory}")
    -->
    <xsl:function name="cmake:add_subdirectory">
        <xsl:param name="directory" as="xs:string" />
        <xsl:value-of select="concat('add_subdirectory(', o:wrap_dblquote($directory), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Finds a library

        # Find library ${library_name}
        find_library(${library_variable} ${library_name} "${library_path}")
    -->
    <xsl:function name="cmake:find_library">
        <xsl:param name="library_variable" as="xs:string" />
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="library_path" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:comment(concat('Find library ', $library_name), $tab)" />
        <xsl:value-of select="concat(o:t($tab), 'find_library(', $library_variable, ' ', $library_name, ' ', o:wrap_dblquote($library_path), ')', o:nl(1))" />
    </xsl:function>

    <!--
        Produces a find_package line

        # Find ${package_name}
        find_package(${package_name} ${args})   
    -->
    <xsl:function name="cmake:find_package">
        <xsl:param name="package_name" as="xs:string" />
        <xsl:param name="args" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:comment(concat('Find ', $package_name), $tab)" />
        <xsl:value-of select="concat(o:t($tab), 'find_package', o:wrap_bracket(o:join_list(($package_name, $args), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Sets a variable
        set(${variable} "${value}")
    -->
    <xsl:function name="cmake:set_variable" as="xs:string">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />
        

        <xsl:value-of select="concat(o:t($tab), 'set(', $variable, ' ', o:wrap_dblquote($value), ')', o:nl(1))" />
    </xsl:function>
    
    <!--
       Includes a .cmake file 
    -->
    <xsl:function name="cmake:include" as="xs:string">
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="concat(o:t($tab), 'include(', $value, ')', o:nl(1))"/>
    </xsl:function>

    <!--
        Includes sem_exports.cmake
    -->
    <xsl:function name="cmake:include_sem_exports" as="xs:string">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), cmake:include('${SEM_LIB_DIR}/sem_exports.cmake', $tab))"/>
    </xsl:function>


    <!--
        Includes zmq exports
    -->
    <xsl:function name="cmake:include_zmq_exports" as="xs:string">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), cmake:include('${SEM_DIR}/build/_deps/zeromq-build/ZeroMQTargets.cmake', $tab))"/>
    </xsl:function>

    <!--
        Includes proto generate function
    -->
    <xsl:function name="cmake:include_proto_generate" as="xs:string">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), cmake:include('${SEM_DIR}/extern/grpc/generate_protobuf.cmake', $tab))"/>
    </xsl:function>
    <!--
        Includes cppzmq exports
    -->
    <xsl:function name="cmake:include_cppzmq_exports" as="xs:string">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), cmake:include('${SEM_DIR}/build/_deps/cppzmq-build/cppzmqTargets.cmake', $tab))"/>
    </xsl:function>

    <!--
        Includes protobuf exports
    -->
    <xsl:function name="cmake:include_protobuf_exports" as="xs:string">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), cmake:include('${SEM_DIR}/build/_deps/grpc-build/third_party/protobuf/lib64/cmake/protobuf/protobuf-targets.cmake', $tab))"/>
    </xsl:function>

    <!--
        Sets a variable
        if(NOT ${variable})
            set(${variable} "${value}")
        endif(NOT ${variable})
    -->
    <xsl:function name="cmake:set_variable_if_not_set">
        <xsl:param name="variable" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:if_start(concat('NOT ', $variable), $tab)" />
        <xsl:value-of select="cmake:set_variable($variable, $value, $tab + 1)" />
        <xsl:value-of select="cmake:if_end(concat('NOT ', $variable), $tab)" />
    </xsl:function>

    <!--
        Sets the project name

        set(PROJ_NAME "${proj_name}")
        project(${PROJ_NAME})
    -->
    <xsl:function name="cmake:set_project_name">
        <xsl:param name="project_name" as="xs:string" />

        <xsl:value-of select="cmake:set_variable('PROJ_NAME', $project_name, 0)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl(2))" />
    </xsl:function>

    <!--
        Adds a shared library (MODULE/SHARED)
        add_library(${library_name} ${library_type} ${args})
    -->
    <xsl:function name="cmake:add_library" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:param name="library_type" as="xs:string" />
        <xsl:param name="args" as="xs:string" />

        <xsl:value-of select="concat('add_library', o:wrap_bracket(o:join_list((cmake:wrap_variable($library_name), $library_type, $args), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an if statement
        if(${condition})
    -->
    <xsl:function name="cmake:if_start">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'if', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an else if statement
        elseif(${condition})
    -->
    <xsl:function name="cmake:if_elseif">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'elseif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Defines an else statement
        else()
    -->
    <xsl:function name="cmake:if_else">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'else', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <!--
        Ends an if statement
        endif(${condition})
    -->
    <xsl:function name="cmake:if_end">
        <xsl:param name="condition" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'endif', o:wrap_bracket($condition), o:nl(1))" />
    </xsl:function>

    <!--
        Prints a CMake message when processed
        message(STATUS ${message})
    -->
    <xsl:function name="cmake:message">
        <xsl:param name="message" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'message', o:wrap_bracket(o:join_list(('STATUS', $message), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Prints a CMake warning when processed
        message(WARNING ${message})
    -->
    <xsl:function name="cmake:warning">
        <xsl:param name="message" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer"/>

        <xsl:value-of select="concat(o:t($tab), 'message', o:wrap_bracket(o:join_list(('WARNING', $message), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Returns out of a CMake function
        return()
    -->
    <xsl:function name="cmake:return">
        <xsl:param name="tab" as="xs:integer"/>
        <xsl:value-of select="concat(o:t($tab), 'return', o:wrap_bracket(''), o:nl(1))" />
    </xsl:function>

    <!--
        Copys a file into a directory
        configure_file(${input_file} ${output_dir} COPYONLY)
    -->
    <xsl:function name="cmake:configure_file">
        <xsl:param name="input_file" as="xs:string"/>
        <xsl:param name="output_dir" as="xs:string"/>
        <xsl:value-of select="concat('configure_file', o:wrap_bracket(o:join_list(($input_file, $output_dir, 'COPYONLY'), ' ')), o:nl(1))" />
    </xsl:function>

    <!--
        Gets the current binary directory variable
        ${CMAKE_CURRENT_BINARY_DIR}
    -->
    <xsl:function name="cmake:current_binary_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_BINARY_DIR')" />
    </xsl:function>

    <!--
        Gets the current source directory variable
        ${CMAKE_CURRENT_SOURCE_DIR}
    -->
    <xsl:function name="cmake:current_source_dir_var">
        <xsl:value-of select="cmake:wrap_variable('CMAKE_CURRENT_SOURCE_DIR')" />
    </xsl:function>

    <!--
        Gets CMakeLists.txt
        CMakeLists.txt
    -->
    <xsl:function name="cmake:cmake_file" as="xs:string">
        <xsl:value-of select="'CMakeLists.txt'" />
    </xsl:function>

    <!--
        Adds a list of sub_directories, will prepend on the current source dir
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${sub_directory_1}")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/${sub_directory_2}")
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
        cmake_minimum_required(VERSION ${version})
    -->
    <xsl:function name="cmake:cmake_minimum_required">
        <xsl:param name="version" as="xs:string"/>
        <xsl:value-of select="concat('cmake_minimum_required(VERSION ', $version, ')', o:nl(1))" />
    </xsl:function>

    <!--
        Sets the required settings to force C++17 Compliance
        # CMake C++17 Options
        set(CMAKE_CXX_STANDARD "17")
        set(CMAKE_CXX_STANDARD_REQUIRED "ON")
        set(CMAKE_CXX_EXTENSIONS "OFF")
        set(CMAKE_POSITION_INDEPENDENT_CODE "ON")
        Sets the required settings to force C++17 Compliance
    -->
    <xsl:function name="cmake:set_cpp17">
        <xsl:value-of select="cmake:comment('CMake C++17 Options', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD', '17', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_STANDARD_REQUIRED', 'ON', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_CXX_EXTENSIONS', 'OFF', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_POSITION_INDEPENDENT_CODE', 'ON', 0)" />
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
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_directory}")
        if(MSVC)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endif(MSVC)
    -->
    <xsl:function name="cmake:set_runtime_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_RUNTIME_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <!--
        Sets the library output directory to the provided ${output_directory}
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${output_directory}")
        if(MSVC)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endif(MSVC)
    -->
    <xsl:function name="cmake:set_library_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_LIBRARY_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <!--
        Sets the archive output directory to the provided ${output_directory}
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${output_directory}")
        if(MSVC)
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
        endif(MSVC)
    -->
    <xsl:function name="cmake:set_archive_output_directory">
        <xsl:param name="output_directory" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:set_output_directory('CMAKE_ARCHIVE_OUTPUT_DIRECTORY', $output_directory, $tab)" />
    </xsl:function>

    <!--
        Sets a cmake property
        set_property(${property})
    -->
    <xsl:function name="cmake:set_property">
        <xsl:param name="property" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'set_property(', $property, ')', o:nl(1))" />
    </xsl:function>

    <!--
        Gets a system environment variable
        $ENV{variable_name}
    -->
    <xsl:function name="cmake:get_env_var" as="xs:string">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:value-of select="concat('$ENV', o:wrap_curly($variable_name))" />
    </xsl:function>

    <!--
        Sets a project to use ccache if available
        # Using CCache speeds up repeat compilation time
        find_program(CCACHE_FOUND ccache)
        if(CCACHE_FOUND)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
            set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
        endif(CCACHE_FOUND)
    -->
    <xsl:function name="cmake:use_ccache">
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="cmake:comment('Using CCache speeds up repeat compilation time', $tab)" />
        <xsl:value-of select="concat(o:t($tab), 'find_program(CCACHE_FOUND ccache)', o:nl(1))" />
        <xsl:value-of select="cmake:if_start('CCACHE_FOUND', $tab)" />
        <xsl:value-of select="cmake:set_property('GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache', $tab + 1)" />
        <xsl:value-of select="cmake:set_property('GLOBAL PROPERTY RULE_LAUNCH_LINK ccache', $tab + 1)" />
        <xsl:value-of select="cmake:if_end('CCACHE_FOUND', $tab)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <!--
        Sets the minimum CMake version required
        install(TARGETS ${PROJ_NAME} DESTINATION lib)
    -->
    <xsl:function name="cmake:install">
        <xsl:param name="target" as="xs:string"/>
        <xsl:param name="destination" as="xs:string"/>
        <xsl:param name="tab" as="xs:integer" />
        <xsl:value-of select="concat(o:t($tab), 'install(TARGETS ', cmake:wrap_variable($target), ' DESTINATION ', $destination, ')', o:nl(1))" />
    </xsl:function>
</xsl:stylesheet>