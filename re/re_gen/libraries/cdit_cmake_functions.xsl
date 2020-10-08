<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:cmake="http://github.com/cdit-ma/re_gen/cmake"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    xmlns:cdit="http://github.com/cdit-ma/re_gen/cdit"
    xmlns:graphml="http://github.com/cdit-ma/re_gen/graphml"
    >

    <xsl:function name="cmake:get_middleware_package">
        <xsl:param name="middleware" as="xs:string"/>

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        
        <xsl:choose>
            <xsl:when test="$middleware_lc = 'rti'">
                <xsl:value-of select="'RTI_DDS'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'ospl'">
                <xsl:value-of select="'OSPL_DDS'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'qpid' or $middleware_lc ='amqp'">
                <xsl:value-of select="'QPID'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'zmq'">
                <xsl:value-of select="'ZMQ'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'tao'">
                <xsl:value-of select="'TAO'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'proto'">
                <xsl:value-of select="'Protobuf'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:message>Warning: Unknown Middleware<xsl:value-of select="$middleware" /></xsl:message>
                <xsl:value-of select="''" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cmake:find_middleware_package">
        <xsl:param name="middleware" />

        <xsl:choose>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="o:nl(1)" />
            </xsl:when>
            <xsl:when test="$middleware = 'zmq'">
                <xsl:value-of select="o:nl(1)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="package_name" select="cmake:get_middleware_package($middleware)" />

                <xsl:value-of select="o:nl(1)" />
                <xsl:value-of select="cmake:if_start(concat('NOT ', upper-case($package_name), '_FOUND'), 0)" />
                <xsl:value-of select="cmake:find_package($package_name, '', 1)" />
                <xsl:value-of select="cmake:if_start(concat('NOT ', upper-case($package_name), '_FOUND'), 1)" />
                <xsl:value-of select="cmake:message(o:wrap_dblquote(o:join_list(('Cannot find', $package_name, 'cannot build', cmake:wrap_variable('PROJ_NAME')), ' ')), 2)" />
                <xsl:value-of select="cmake:return(2)" />
                <xsl:value-of select="cmake:if_end('', 1)" />
                <xsl:value-of select="cmake:if_end('', 0)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:otherwise>
        </xsl:choose>
        

    </xsl:function>

    <xsl:function name="cmake:get_middleware_helper_var">
        <xsl:param name="middleware" />

        <xsl:variable name="package_name" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="helper_lib" select="lower-case(concat($package_name, '_helper'))" />
        <xsl:variable name="helper_var" select="concat(upper-case($helper_lib), '_LIBRARIES')" />

        <xsl:value-of select="$helper_var" />
    </xsl:function>


    <xsl:function name="cmake:find_middleware_helper">
        <xsl:param name="middleware" />

        <xsl:choose>
            <xsl:when test="$middleware = 'zmq'">
                <xsl:value-of select="o:nl(1)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="package_name" select="cmake:get_middleware_package($middleware)" />
                <xsl:variable name="helper_lib" select="lower-case(concat($package_name, '_helper'))" />
                <xsl:variable name="helper_var" select="cmake:get_middleware_helper_var($middleware)" />

                <xsl:value-of select="cmake:find_library_safe($helper_var, $helper_lib, cmake:get_re_lib_path(''))" />
            </xsl:otherwise>
        </xsl:choose>

    </xsl:function>

    <xsl:function name="cmake:find_library_safe">
        <xsl:param name="lib_var" as="xs:string" />
        <xsl:param name="lib_name" as="xs:string" />
        <xsl:param name="lib_path" as="xs:string" />

        <xsl:value-of select="cmake:if_start(concat('NOT ', $lib_var), 0)" />
        <xsl:value-of select="cmake:find_library($lib_var, $lib_name , $lib_path , 1)" />
        <xsl:value-of select="cmake:if_start(concat('NOT ', $lib_var), 1)" />
        <xsl:value-of select="cmake:warning(o:wrap_dblquote(o:join_list(('Cannot find', $lib_name, 'cannot build', cmake:wrap_variable('PROJ_NAME')), ' ')), 2)" />
        <xsl:value-of select="cmake:return(2)" />
        <xsl:value-of select="cmake:if_end('', 1)" />
        <xsl:value-of select="cmake:if_end('', 0)" />
        <xsl:value-of select="o:nl(1)" /> 
    </xsl:function>


    <xsl:function name="cmake:find_re_core_library">
        <!-- Find re_core -->
        <xsl:variable name="lib_path" select="cmake:get_re_lib_path('')" />

        <xsl:value-of select="cmake:find_library('RE_CORE_LIBRARIES', 're_core', $lib_path, 0)" />
        <xsl:value-of select="cmake:find_library('RE_SINGLETON_LIBRARIES', 're_core_singletons', $lib_path, 0)" />

        <xsl:value-of select="cmake:if_start('NOT RE_CORE_LIBRARIES OR NOT RE_SINGLETON_LIBRARIES', 0)" />
        <xsl:value-of select="cmake:message(o:wrap_dblquote(o:join_list(('Cannot find re_core cannot build', cmake:wrap_variable('PROJ_NAME')), ' ')), 1)" />
        <xsl:value-of select="cmake:return(1)" />
        <xsl:value-of select="cmake:if_end('', 0)" />
        <xsl:value-of select="o:nl(1)" /> 
    </xsl:function>


    <xsl:function name="cdit:build_module_library" as="xs:boolean">
        <xsl:param name="middleware" />

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware_lc = 'proto' or $middleware_lc = 'base'">
                <xsl:value-of select="false()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="true()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:build_shared_library" as="xs:boolean">
        <xsl:param name="middleware" />

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware_lc = 'proto' or $middleware_lc = 'rti' or $middleware = 'ospl' or $middleware_lc = 'tao' or $middleware_lc = 'base'">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cmake:get_re_path" as="xs:string">
        <xsl:param name="prefix" />
        <xsl:value-of select="o:join_paths((cmake:wrap_variable('RE_PATH'), $prefix))" />
    </xsl:function>

    <xsl:function name="cmake:get_re_lib_path" as="xs:string">
        <xsl:param name="prefix" />
        <xsl:value-of select="o:join_paths((cmake:wrap_variable('RE_LIB_PATH'), $prefix))" />
    </xsl:function>

    <xsl:function name="cmake:get_relative_path" as="xs:string">
        <xsl:param name="path_list" as="xs:string*" />

        <xsl:variable name="directory_spaces" as="xs:string*">
            <xsl:for-each select="$path_list">
                <xsl:if test=". != ''">
                    <xsl:value-of select="'..'" />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="o:join_paths($directory_spaces)" />
    </xsl:function>


    <xsl:function name="cmake:get_middleware_generated_header_var" as="xs:string">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:value-of select="concat(upper-case($middleware), '_HEADER')" />
    </xsl:function>

    <xsl:function name="cmake:get_middleware_generated_source_var" as="xs:string">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:value-of select="concat(upper-case($middleware), '_SOURCES')" />
    </xsl:function>

    <xsl:function name="cmake:target_include_middleware_directories">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:choose>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="o:nl(1)"/>
            </xsl:when>
            <xsl:when test="$middleware = 'zmq'">
                <xsl:value-of select="o:nl(1)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
                <xsl:variable name="include_directory" select="concat($middleware_package, '_INCLUDE_DIRS')" />

                <xsl:if test="$middleware_package != ''">
                    <xsl:value-of select="cmake:target_include_directories($target, 'PUBLIC', cmake:wrap_variable($include_directory), $tab)" />
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cmake:target_link_middleware_libraries">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:choose>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="cmake:target_link_libraries($target, 'PUBLIC', 'protobuf::libprotobuf', $tab)"/>
            </xsl:when>
            <xsl:when test="$middleware = 'zmq'">
                <xsl:value-of select="cmake:target_link_libraries($target, 'PUBLIC', 'libzmq', $tab)"/>
                <xsl:value-of select="cmake:target_link_libraries($target, 'PUBLIC', 'cppzmq', $tab)"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
                <xsl:variable name="include_directory" select="concat($middleware_package, '_LIBRARIES')" />

                <xsl:if test="$middleware_package != ''">
                    <xsl:value-of select="cmake:target_link_libraries($target, 'PUBLIC', cmake:wrap_variable($include_directory), $tab)" />
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>

    </xsl:function>

    <xsl:function name="cmake:generate_middleware_compiler">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="required_aggregates" as="element()*" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="middleware_extension" select="cdit:get_middleware_extension($middleware)" />

        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment(('Inform middleware compiler', o:wrap_angle($middleware_extension), 'of directories to look for relative files.'), 0)" />
                <xsl:value-of select="concat('set(', upper-case(concat($middleware_package, '_IMPORT_DIRS')), o:nl(1))" />
            </xsl:if>
            <xsl:variable name="import_path" select="o:join_paths((cmake:wrap_variable('MODEL_SOURCE_DIR'), 'datatypes', $middleware, cdit:get_namespace_type_path(.)))" />
            <xsl:value-of select="concat(o:t(1), $import_path, o:nl(1))" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Run the middlewares compiler over the middleware file-->
        <xsl:variable name="middleware_file" select="o:join_paths((cmake:current_source_dir_var(), cdit:get_aggregates_middleware_file_name($aggregate, $middleware)))" />

        <xsl:variable name="source" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="header" select="cmake:get_middleware_generated_header_var($middleware)" />


        <xsl:variable name="middleware_compiler" select="upper-case(concat($middleware_package, '_generate_cpp'))" />

        
        <xsl:value-of select="cmake:comment(('Run the', $middleware, 'Compiler over the', o:wrap_angle($middleware_extension), 'files'), 0)" />
        <xsl:variable name="compiler_args" as="xs:string*">
            <xsl:sequence select="$source" />
            <xsl:sequence select="$header" />
            <xsl:if test="$middleware = 'proto'">
                <!-- Test Out protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS EXPORT_MACRO DLL_EXPORT foo.proto) 
                <xsl:sequence select="'EXPORT_MACRO'" />
                <xsl:sequence select="'__attribute__((visibility("default")))'" />-->
            </xsl:if>
            <xsl:sequence select="$middleware_file" />
        </xsl:variable>
        <xsl:variable name="compiler_arg" select="o:join_list($compiler_args, ' ')" />

        <xsl:choose>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="concat('CDIT_MA_GENERATE_PROTOBUF_CPP', o:wrap_bracket($compiler_arg), o:nl(1))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat($middleware_compiler, o:wrap_bracket($compiler_arg), o:nl(1))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cmake:combine_project_library_name" as="xs:string">
        <xsl:param name="library_name" as="xs:string" />
        <xsl:value-of select="o:join_list((cmake:get_library_prefix(), lower-case($library_name)), '_')" />
    </xsl:function>

    <xsl:function name="cmake:get_aggregate_shared_library_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($aggregate)" />

        <xsl:variable name="shared_middleware">
             <xsl:choose>
                <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                    <xsl:value-of select="'proto'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$middleware" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="library_name" select="o:join_list(('datatype', $shared_middleware, $namespace, $label), '_')" />
        <xsl:value-of select="cmake:combine_project_library_name($library_name)" />
    </xsl:function>


    <xsl:function name="cmake:get_pubsub_module_library_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="library_name" select="o:join_list(('port', 'pubsub', $middleware, $namespace, $label), '_')" />
        <xsl:value-of select="cmake:combine_project_library_name($library_name)" />
    </xsl:function>

    <xsl:function name="cmake:get_requestreply_module_library_name" as="xs:string">
        <xsl:param name="server_interface" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="label" select="graphml:get_label($server_interface)" />
        <xsl:variable name="namespace" select="graphml:get_namespace($server_interface)" />
        <xsl:variable name="library_name" select="o:join_list(('port', 'requestreply', $middleware, $namespace, $label), '_')" />
        <xsl:value-of select="cmake:combine_project_library_name($library_name)" />
    </xsl:function>

    

    

    <xsl:function name="cmake:target_link_custom_libraries">
        <xsl:param name="entity" as="element()" />

        <!-- Include the headers once for each worker type -->
        <xsl:for-each-group select="graphml:get_custom_class_instances($entity)" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Target Link Custom Class', 0)" />
            </xsl:if>
            <xsl:variable name="worker_lib_name" select="cmake:get_class_shared_library_name(.)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $worker_lib_name, 0)" />

            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cmake:target_link_worker_libraries">
        <xsl:param name="entity" as="element()" />

        <!-- Include the headers once for each worker type -->
        <xsl:for-each-group select="graphml:get_worker_instances($entity)" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Include Worker Header Files', 0)" />
            </xsl:if>
            <xsl:variable name="worker_def" select="graphml:get_definition(.)" />

            <xsl:variable name="worker_lib_name" select="graphml:get_data_value($worker_def, 'file')" />

            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', concat('sem::', $worker_lib_name), 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cmake:target_link_aggregate_libraries">
        <xsl:param name="entity" as="element()" />

        <xsl:variable name="source_dir_var" select="cmake:current_source_dir_var()" />

        <!-- Include the headers once for each worker type -->
        <xsl:for-each-group select="cdit:get_required_aggregates($entity, true())" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Include Aggregate Libraries', 0)" />
            </xsl:if>

            <xsl:variable name="required_lib_name" select="cmake:get_aggregate_shared_library_name(., 'base')" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $required_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cmake:include_top_level_source_dir">
        <xsl:param name="relative_path" as="xs:string" />
        <xsl:param name="proj_name" as="xs:string" />

        <xsl:value-of select="cmake:comment('Include Top Level Source Directory', 0)" />
        <xsl:variable name="required_path" select="o:join_paths((cmake:current_source_dir_var(), $relative_path))" />
        <xsl:value-of select="cmake:target_include_directories($proj_name, 'PUBLIC', $required_path, 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cmake:include_top_level_binary_dir">
        <xsl:param name="relative_path" as="xs:string" />
        <xsl:param name="proj_name" as="xs:string" />

        <xsl:value-of select="cmake:comment('Include Top Level Binary Directory', 0)" />
        <xsl:variable name="required_path" select="o:join_paths((cmake:current_binary_dir_var(), $relative_path))" />
        <xsl:value-of select="cmake:target_include_directories($proj_name, 'PUBLIC', $required_path, 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>


    <xsl:function name="cdit:get_datatype_relative_path_to_top_level">
        <xsl:param name="aggregate"/>
        <xsl:value-of select="cmake:get_relative_path(('datatypes', 'middleware', graphml:get_namespace($aggregate), 'LABEL'))" />
    </xsl:function>

    <xsl:function name="cdit:get_port_relative_path_to_top_level">
        <xsl:param name="aggregate"/>
        <xsl:value-of select="cmake:get_relative_path(('ports', 'requestreply', 'middleware', graphml:get_namespace($aggregate), 'LABEL'))" />
    </xsl:function>

    
    <!--
        Gets the translate_cmake file
    -->
    <xsl:function name="cdit:get_enum_cmake">
        <xsl:param name="enum"/>
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="proj_name" select="cmake:get_aggregate_shared_library_name($enum, $middleware)" />
                
        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />


        <xsl:variable name="middleware_sources" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="middleware_headers" select="cmake:get_middleware_generated_header_var($middleware)" />

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('cdit_cmake_functions.xsl', 'cdit:get_enum_cmake', 0)" />
        <xsl:value-of select="cmake:set_project_name($proj_name)" />
        <xsl:value-of select="o:nl(1)" />
        
        <!-- Find the Middleware specific package -->
        <xsl:value-of select="cmake:find_middleware_package($middleware)" />
        <xsl:value-of select="cmake:generate_middleware_compiler($enum, (), $middleware)" />
        
        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable($middleware_sources), cmake:wrap_variable($middleware_headers)), ' ')" />
        <xsl:value-of select="cmake:add_library('PROJ_NAME', 'SHARED', $args)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Target Include Directories -->
        <xsl:value-of select="cmake:comment('Include the middleware include directory', 0)" />
        <xsl:value-of select="cmake:target_include_middleware_directories('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Set RTI specific settings -->
        <xsl:if test="$middleware = 'rti'">
            <xsl:value-of select="cmake:comment('Set RTI Definitions', 0)" />
            <xsl:variable name="rti_definitions" select="cmake:wrap_variable(concat(cmake:get_middleware_package($middleware), '_DEFINITIONS'))" />

            <xsl:value-of select="cmake:target_compile_definitions('PROJ_NAME', 'PUBLIC', $rti_definitions, 0)" />
        </xsl:if>

        <!-- Target Link Libraries -->
        <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
        <xsl:value-of select="cmake:target_link_middleware_libraries('PROJ_NAME', $middleware, 0)" />
    </xsl:function>

    <!--
        Gets the translate_cmake file
    -->
    <xsl:function name="cdit:get_datatype_cmake">
        <xsl:param name="aggregate"/>
        <xsl:param name="middleware" as="xs:string" />
        
        
        <!-- Get all required aggregates -->
        <xsl:variable name="linkable_aggregates" select="cdit:get_required_aggregates($aggregate, true())" />
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($aggregate, false())" />
        <xsl:variable name="required_enums" select="cdit:get_required_enums($aggregate)" />

        <xsl:variable name="include_datatypes" select="($required_aggregates, $required_enums)" />
        <xsl:variable name="linkable_datatypes" select="($linkable_aggregates, $required_enums)" />
        
        
        <xsl:variable name="proj_name" select="cmake:get_aggregate_shared_library_name($aggregate, $middleware)" />
        
        <xsl:variable name="base_lib_name" select="cmake:get_aggregate_shared_library_name($aggregate, 'base')" />

        <xsl:variable name="middleware_sources" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="middleware_headers" select="cmake:get_middleware_generated_header_var($middleware)" />
        
        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />

        <xsl:variable name="middleware_extension" select="cdit:get_middleware_extension($middleware)" />


        <xsl:variable name="binary_dir_var" select="cmake:current_binary_dir_var()" />
        <xsl:variable name="source_dir_var" select="cmake:current_source_dir_var()" />

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('cdit_cmake_functions.xsl', 'cdit:get_datatype_cmake', 0)" />
        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <xsl:value-of select="cmake:find_package('Boost', 'COMPONENTS thread system filesystem REQUIRED', 0)" />

        <!-- Find the Middleware specific package -->
        <xsl:value-of select="cmake:find_middleware_package($middleware)" />

        <!-- Include export files -->
        <xsl:value-of select="cmake:include_zmq_exports(0)"/>
        <xsl:value-of select="cmake:include_cppzmq_exports(0)"/>
        <xsl:value-of select="cmake:include_protobuf_exports(0)"/>
        <xsl:value-of select="cmake:include_sem_exports(0)"/>

        <!-- Run the Generator -->
        <xsl:choose>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="cmake:include_proto_generate(0)"/>
                <xsl:value-of select="cmake:generate_middleware_compiler($aggregate, $include_datatypes, $middleware)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cmake:generate_middleware_compiler($aggregate, $include_datatypes, $middleware)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:otherwise>
        </xsl:choose>

        <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), $source_dir_var, '/translator.cpp', o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />


        <!-- Create library -->
        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS'), cmake:wrap_variable($middleware_sources), cmake:wrap_variable($middleware_headers)), ' ')" />
        <xsl:value-of select="cmake:add_library('PROJ_NAME', 'STATIC', $args)" />
        <xsl:value-of select="o:nl(1)" />

        

        <!-- Include -->
        <xsl:value-of select="cmake:comment('Include Top Level Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PUBLIC', cmake:wrap_variable('MODEL_SOURCE_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PUBLIC', cmake:wrap_variable('MODEL_BINARY_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PUBLIC', cmake:wrap_variable('RE_SOURCE_DIR'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Runtime Environment -->
        <xsl:value-of select="cmake:comment('Link against re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core_singletons', 0)" />
        <xsl:value-of select="o:nl(1)" />


        <!-- Include Middleware Directories -->
        <xsl:value-of select="cmake:comment('Include the middleware include directory', 0)" />
        <xsl:value-of select="cmake:target_include_middleware_directories('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="o:nl(1)" />

        
        <!-- Include Binary Directory -->
        <xsl:value-of select="cmake:comment('Include the current binary directory to allow inclusion of generated files', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PUBLIC', $binary_dir_var, 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include required binary directories -->
        <xsl:for-each select="$linkable_datatypes">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment(('Include required binary directories.'), 0)" />
            </xsl:if>
            <xsl:variable name="aggregate_dir" select="o:join_paths((cmake:wrap_variable('MODEL_BINARY_DIR'), 'datatypes', $middleware, cdit:get_namespace_type_path(.)))" />
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PUBLIC', $aggregate_dir, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Set RTI specific settings -->
        <xsl:if test="$middleware = 'rti'">
            <xsl:value-of select="cmake:comment('Set RTI Definitions', 0)" />
            <xsl:variable name="rti_definitions" select="cmake:wrap_variable(concat(cmake:get_middleware_package($middleware), '_DEFINITIONS'))" />

            <xsl:value-of select="cmake:target_compile_definitions('PROJ_NAME', 'PUBLIC', $rti_definitions, 0)" />
        </xsl:if>

        <!-- Target Link Libraries -->
        <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
        <xsl:value-of select="cmake:target_link_middleware_libraries('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="o:nl(1)" />


        <xsl:if test="$middleware != 'base'">
            <xsl:value-of select="cmake:comment('Link against the base aggregate', 0)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $base_lib_name, 0)" />
        </xsl:if>
            
        <!-- Link against required binaries -->
        <xsl:for-each select="$linkable_datatypes">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment(('Link against', o:wrap_angle($middleware_extension), 'libraries'), 0)" />
            </xsl:if>
            <xsl:variable name="required_lib" select="cmake:get_aggregate_shared_library_name(., $middleware)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $required_lib, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>


    <!--
        Gets the get_pubsub_cmake file
    -->
    <xsl:function name="cdit:get_pubsub_cmake">
        <xsl:param name="aggregate"/>
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="proj_name" select="cmake:get_pubsub_module_library_name($aggregate, $middleware)" />
        <xsl:variable name="datatype_lib_name" select="cmake:get_aggregate_shared_library_name($aggregate, $middleware)" />

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('cdit_cmake_functions.xsl', 'cdit:get_pubsub_cmake', 0)" />
        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find boost -->
        <xsl:value-of select="cmake:find_package('Boost', 'COMPONENTS thread system filesystem REQUIRED', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Find the Middleware specific package -->
        <xsl:value-of select="cmake:find_middleware_package($middleware)" />

        <!-- Find the middleware helper -->
        <xsl:value-of select="cmake:find_middleware_helper($middleware)" />

        <!-- Include export files -->
        <xsl:value-of select="cmake:include_zmq_exports(0)"/>
        <xsl:value-of select="cmake:include_cppzmq_exports(0)"/>
        <xsl:value-of select="cmake:include_protobuf_exports(0)"/>
        <xsl:value-of select="cmake:include_sem_exports(0)"/>

        <!-- Add the Library -->
        <xsl:value-of select="cmake:add_library('PROJ_NAME', 'MODULE', o:join_paths((cmake:current_source_dir_var(), 'libportexport.cpp')))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Install -->
        <xsl:value-of select="cmake:comment('Install', 0)" />
        <xsl:value-of select="cmake:install('PROJ_NAME', 'lib', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Runtime Environment -->
        <xsl:value-of select="cmake:comment('Include Top Level Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('MODEL_SOURCE_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('MODEL_BINARY_DIR'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Middleware Directories -->
        <xsl:value-of select="cmake:comment('Include Middleware Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_middleware_directories('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Link Runtime Environment -->
        <xsl:value-of select="cmake:comment('Link against re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::re_core_singletons', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::prototranslator', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::zmq_helper', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'sem::zmq_utils', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Target Link Libraries -->
        <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
        <xsl:value-of select="cmake:target_link_middleware_libraries('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable(cmake:get_middleware_helper_var($middleware)), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Link against Boost -->
        <xsl:value-of select="cmake:comment('Link against Boost', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'Boost::system', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', 'Boost::thread', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cmake:comment('Link against the base aggregate', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', $datatype_lib_name, 0)" />
    </xsl:function>

    <!--
        Gets the get_reqrep_cmake file
    -->
    <xsl:function name="cdit:get_requestreply_cmake">
        <xsl:param name="server_interface"/>
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="ignore_recursion" select="$middleware != 'tao'" />
        
        <!-- Get all required aggregates -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($server_interface, $ignore_recursion)" />

        <xsl:variable name="proj_name" select="cmake:get_requestreply_module_library_name($server_interface, $middleware)" />

        <xsl:variable name="source_dir_var" select="cmake:current_source_dir_var()" />

        <!-- Version Number -->
        <xsl:value-of select="cmake:print_regen_version('cdit_cmake_functions.xsl', 'cdit:get_requestreply_cmake', 0)" />
        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_re_core_library()" />

        <!-- Find boost -->
        <xsl:value-of select="cmake:find_package('Boost', 'COMPONENTS thread system REQUIRED', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Find the Middleware specific package -->
        <xsl:value-of select="cmake:find_middleware_package($middleware)" />

        <!-- Find the middleware helper -->
        <xsl:value-of select="cmake:find_middleware_helper($middleware)" />

        <xsl:if test="$middleware = 'tao'">
            <xsl:variable name="required_enums" select="cdit:get_required_enums($server_interface)" />
            <xsl:variable name="required_datatypes" select="(($required_aggregates, $required_enums))" />
            <!-- Run the middlewares compiler over the middleware file-->
            <xsl:value-of select="cmake:generate_middleware_compiler($server_interface, $required_datatypes, $middleware)" />
        </xsl:if>

         <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), $source_dir_var, '/libportexport.cpp', o:nl(1))" />
        <xsl:if test="$middleware = 'tao'">
            <xsl:value-of select="concat(o:t(1), cmake:wrap_variable('TAO_SOURCES'), o:nl(1))" />
            <xsl:value-of select="concat(o:t(1), cmake:wrap_variable('TAO_HEADER'), o:nl(1))" />
        </xsl:if>
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Add the Library -->
        <xsl:value-of select="cmake:add_library('PROJ_NAME', 'MODULE', cmake:wrap_variable('SOURCE'))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Install -->
        <xsl:value-of select="cmake:comment('Install', 0)" />
        <xsl:value-of select="cmake:install('PROJ_NAME', 'lib', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Runtime Environment -->
        <xsl:value-of select="cmake:comment('Include Top Level Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('MODEL_SOURCE_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('MODEL_BINARY_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('RE_SOURCE_DIR'), 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('RE_COMMON_SOURCE_DIR'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include Middleware Directories -->
        <xsl:value-of select="cmake:comment('Include Middleware Dirs', 0)" />
        <xsl:value-of select="cmake:target_include_middleware_directories('PROJ_NAME', $middleware, 0)" />
        <xsl:if test="$middleware = 'tao'">
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', 'PRIVATE', cmake:current_binary_dir_var(), 0)" />
        </xsl:if>
        <xsl:value-of select="o:nl(1)" />


        <!-- Link Runtime Environment -->
        <xsl:value-of select="cmake:comment('Link against re_core', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('RE_SINGLETON_LIBRARIES'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Target Link Libraries -->
        <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
        <xsl:value-of select="cmake:target_link_middleware_libraries('PROJ_NAME', $middleware, 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable(cmake:get_middleware_helper_var($middleware)), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Link against Boost -->
        <xsl:value-of select="cmake:comment('Link against Boost', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('Boost_SYSTEM_LIBRARY'), 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PRIVATE', cmake:wrap_variable('Boost_THREAD_LIBRARY'), 0)" />
        <xsl:value-of select="o:nl(1)" />
        
        <!-- Include required binary directories -->
        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
            </xsl:if>
            <xsl:variable name="datatype_lib_name" select="cmake:get_aggregate_shared_library_name(., cdit:get_datatype_middleware($middleware))" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', 'PUBLIC', $datatype_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    

    <xsl:function name="cdit:get_datatype_middleware">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:choose>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="'proto'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$middleware" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!-- 
        Sets up RE_PATH
    -->
    <xsl:function name="cmake:setup_re_path">
        <xsl:param name="tab" as="xs:integer" />

        <xsl:value-of select="cmake:comment('CDIT Runtime Paths', $tab)" />
        <xsl:value-of select="cmake:set_variable_if_not_set('SEM_DIR', cmake:get_env_var('SEM_DIR'), $tab)" />
        <xsl:value-of select="cmake:set_variable_if_not_set('SEM_LIB_DIR', cmake:get_env_var('SEM_LIB_DIR'), $tab)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_MODULE_PATH', cmake:get_env_var('SEM_CMAKE_MODULE_PATH'), $tab)" />
    </xsl:function>

    <!--
        Gets the top level CMakeLists file
    -->
    <xsl:function name="cmake:get_top_cmakelists">
        <xsl:value-of select="cmake:print_regen_version('cdit_cmake_functions.xsl', 'cdit:get_top_cmakelists', 0)" />
        <xsl:value-of select="cmake:cmake_minimum_required('3.1')" />
        <xsl:value-of select="cmake:set_project_name('generated_model')" />
        <xsl:value-of select="cmake:set_cpp17()" />
        
        <!-- Don't set if we have valid values already, If we aren't the top level, don't redefine -->
        <xsl:value-of select="concat('get_directory_property(IS_SUB_REPO PARENT_DIRECTORY)', o:nl(1))" />
        
        <xsl:value-of select="cmake:if_start('NOT IS_SUB_REPO', 0)" />
        <xsl:value-of select="cmake:use_ccache(1)" />
        <xsl:value-of select="cmake:setup_re_path(1)" />
        <xsl:variable name="lib_dir" select="o:join_paths((cmake:current_source_dir_var(), 'lib'))" />
        <xsl:value-of select="cmake:set_library_output_directory($lib_dir, 1)" />
        <xsl:value-of select="cmake:set_archive_output_directory($lib_dir, 1)" />
        <xsl:value-of select="cmake:if_end('NOT IS_SUB_REPO', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Fix for CentOS RPath MED-580 -->
        <xsl:value-of select="cmake:if_start('UNIX', 0)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_INSTALL_RPATH_USE_LINK_PATH', 'TRUE', 1)" />
        <xsl:value-of select="cmake:set_variable('CMAKE_INSTALL_RPATH', '$ORIGIN/:$ORIGIN/../lib', 1)" />
        <xsl:value-of select="cmake:if_end('UNIX', 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Add Top Level Paths -->
        <xsl:value-of select="cmake:comment('Set top level vars', 0)" />
        <xsl:value-of select="cmake:set_variable('RE_DIR', o:join_paths((cmake:wrap_variable('SEM_DIR'), 're')), 0)" />
        <xsl:value-of select="cmake:set_variable('RE_SOURCE_DIR', o:join_paths((cmake:wrap_variable('RE_DIR'), 'src')), 0)" />
        <xsl:value-of select="cmake:set_variable('RE_COMMON_SOURCE_DIR', o:join_paths((cmake:wrap_variable('RE_DIR'), 're_common')), 0)" />
        <xsl:value-of select="cmake:set_variable('MODEL_SOURCE_DIR', cmake:current_source_dir_var(), 0)" />
        <xsl:value-of select="cmake:set_variable('MODEL_BINARY_DIR', cmake:current_binary_dir_var(), 0)" />
        <xsl:value-of select="o:nl(1)" />


        <xsl:value-of select="cmake:add_subdirectories(('components', 'ports', 'classes', 'datatypes'))" />
    </xsl:function>

    <!--
        Gets the Binary Prefix for CMake if one exists
    -->
    <xsl:function name="cmake:get_library_prefix" as="xs:string">
        <xsl:value-of select="$library_prefix" />
    </xsl:function>

    <xsl:function name="cmake:get_class_shared_library_name" as="xs:string">
        <xsl:param name="entity" as="element()" />

        <xsl:variable name="definition" select="graphml:get_definition($entity)" />
        <xsl:variable name="kind" select="graphml:get_kind($definition)" />
        <xsl:variable name="label" select="lower-case(graphml:get_label($definition))" />
        <xsl:variable name="namespace" select="graphml:get_namespace($definition)" />

        <xsl:variable name="library_name" select="o:join_list(($kind, $namespace, $label), '_')" />
        <xsl:value-of select="cmake:combine_project_library_name($library_name)" />
    </xsl:function>

</xsl:stylesheet>