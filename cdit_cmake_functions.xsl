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
            <xsl:when test="$middleware_lc = 'qpid'">
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
    </xsl:function>

    <xsl:function name="cmake:find_middleware_helper">
        <xsl:param name="middleware" />

        <xsl:variable name="package_name" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="helper_lib" select="lower-case(concat($package_name, '_helper'))" />
        <xsl:variable name="helper_var" select="concat(upper-case($helper_lib), '_LIBRARIES')" />

        <xsl:value-of select="cmake:find_library_safe($helper_var, $helper_lib, cmake:get_re_path('lib'))" />
    </xsl:function>

    <xsl:function name="cmake:find_library_safe">
        <xsl:param name="lib_var" as="xs:string" />
        <xsl:param name="lib_name" as="xs:string" />
        <xsl:param name="lib_path" as="xs:string" />
        
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cmake:if_start(concat('NOT ', $lib_var), 0)" />
        <xsl:value-of select="cmake:find_library($lib_var, $lib_name , $lib_path , 1)" />
        <xsl:value-of select="cmake:if_start(concat('NOT ', $lib_var), 1)" />
        <xsl:value-of select="cmake:message(o:wrap_dblquote(o:join_list(('Cannot find', $lib_name, 'cannot build', cmake:wrap_variable('PROJ_NAME')), ' ')), 2)" />
        <xsl:value-of select="cmake:return(2)" />
        <xsl:value-of select="cmake:if_end('', 1)" />
        <xsl:value-of select="cmake:if_end('', 0)" />
        <xsl:value-of select="o:nl(1)" /> 
    </xsl:function>


    <xsl:function name="cmake:find_re_core_library">
        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_library_safe('RE_CORE_LIBRARIES', 're_core', cmake:get_re_path('lib'))" />
    </xsl:function>

    <xsl:function name="cdit:build_module_library" as="xs:boolean">
        <xsl:param name="middleware" />

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware_lc = 'proto' or $middleware_lc = 'base' or $middleware_lc = 'tao'">
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
            <xsl:when test="$middleware_lc = 'proto' or $middleware_lc = 'rti' or $middleware = 'ospl' or $middleware_lc = 'tao'">
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

    



    <xsl:function name="cmake:get_aggregates_middleware_file_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="file_path" select="cdit:get_aggregates_path($aggregate)" />
        <xsl:variable name="file_name" select="cdit:get_aggregates_middleware_file_name($aggregate, $middleware)" />
        
        <xsl:value-of select="o:join_paths(($file_path, $file_name))" />
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

        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="include_directory" select="concat($middleware_package, '_INCLUDE_DIRS')" />

        <xsl:if test="$middleware_package != ''">
            <xsl:value-of select="cmake:target_include_directories($target, cmake:wrap_variable($include_directory), $tab)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cmake:target_link_middleware_libraries">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="include_directory" select="concat($middleware_package, '_LIBRARIES')" />

        <xsl:if test="$middleware_package != ''">
            <xsl:value-of select="cmake:target_link_libraries($target, cmake:wrap_variable($include_directory), $tab)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cmake:generate_middleware_compiler">
        <xsl:param name="file" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="middleware_extension" select="cdit:get_middleware_extension($middleware)" />
        
        <xsl:variable name="source" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="header" select="cmake:get_middleware_generated_header_var($middleware)" />


        <xsl:variable name="middleware_compiler" select="upper-case(concat($middleware_package, '_generate_cpp'))" />

        
        <xsl:value-of select="cmake:comment(('Run the', $middleware, 'Compiler over the', o:wrap_angle($middleware_extension), 'files'), 0)" />
        <xsl:variable name="compiler_args" select="o:join_list(($source, $header, $file), ' ')" />
        <xsl:value-of select="concat($middleware_compiler, o:wrap_bracket($compiler_args), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:get_aggregates_middleware_shared_library_name" as="xs:string">
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
        <xsl:value-of select="lower-case(o:join_list(($shared_middleware, $namespace, $label, 'lib'), '_'))" />
    </xsl:function>


    <xsl:function name="cmake:get_aggregates_middleware_module_library_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:value-of select="lower-case(o:join_list(($middleware, $aggregate_namespace, $aggregate_label), '_'))" />
    </xsl:function>

    

    <xsl:function name="cmake:target_link_custom_libraries">
        <xsl:param name="entity" as="element()" />

        <!-- Include the headers once for each worker type -->
        <xsl:for-each-group select="graphml:get_custom_class_instances($entity)" group-by="graphml:get_definition(.)">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Include Custom Class Header Files', 0)" />
            </xsl:if>
            <xsl:variable name="class_def" select="graphml:get_definition(.)" />

            <xsl:variable name="namespace" select="graphml:get_namespace($class_def)" />
            <xsl:variable name="label" select="graphml:get_label($class_def)" />

            <xsl:variable name="worker_lib_name" select="lower-case(o:join_list(('class', $namespace, $label), '_'))" />

            <xsl:variable name="worker_lib_var" select="upper-case(concat($worker_lib_name, '_LIBRARIES'))" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', $worker_lib_name, 0)" />
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

            <xsl:variable name="worker_lib_var" select="upper-case(concat($worker_lib_name, '_LIBRARIES'))" />
            
            <xsl:value-of select="cmake:find_library($worker_lib_name, $worker_lib_var, cmake:get_re_path('lib'), 0)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable($worker_lib_var), 0)" />
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

            <xsl:variable name="required_lib_name" select="cmake:get_aggregates_middleware_shared_library_name(., 'base')" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', $required_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="cmake:include_top_level_source_dir">
        <xsl:param name="relative_path" as="xs:string" />
        <xsl:param name="proj_name" as="xs:string" />

        <xsl:value-of select="cmake:comment('Include Top Level source dir', 0)" />
        <xsl:variable name="required_path" select="o:join_paths((cmake:current_source_dir_var(), $relative_path))" />
        <xsl:value-of select="cmake:target_include_directories($proj_name, $required_path, 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>
</xsl:stylesheet>