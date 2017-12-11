<!-- Functions for cmake syntax output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:cmake="http://github.com/cdit-ma/cmake"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    >

    <xsl:function name="cmake:get_middleware_package">
        <xsl:param name="middleware" as="xs:string"/>

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        
        <xsl:choose>
            <xsl:when test="$middleware_lc = 'rti'">
                <xsl:value-of select="'RTIDDS'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'ospl'">
                <xsl:value-of select="'OSPL'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'qpid'">
                <xsl:value-of select="'QPID'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'zmq'">
                <xsl:value-of select="'ZMQ'" />
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

   

    <xsl:function name="cmake:get_middleware_extension" as="xs:string">
        <xsl:param name="middleware" as="xs:string"/>

        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        
        <xsl:choose>
            <xsl:when test="$middleware_lc = 'rti' or $middleware_lc = 'ospl'">
                <xsl:value-of select="'idl'" />
            </xsl:when>
            <xsl:when test="$middleware_lc = 'proto'">
                <xsl:value-of select="'proto'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="concat('UNKNOWN_MIDDLEWARE_', $middleware)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    

    <xsl:function name="cmake:find_middleware_package">
        <xsl:param name="middleware" />
        
        <xsl:variable name="package_name" select="cmake:get_middleware_package($middleware)" />
        
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cmake:find_package($package_name, '')" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cmake:if_start(concat('NOT ', upper-case($package_name), '_FOUND'), 0)" />
        <xsl:value-of select="cmake:message(o:wrap_dblquote(o:join_list(('Cannot find', $package_name, 'cannot build', cmake:wrap_variable('PROJ_NAME')), ' ')), 1)" />
        <xsl:value-of select="cmake:return(1)" />
        <xsl:value-of select="cmake:if_end('', 0)" />
        <xsl:value-of select="o:nl(1)" />
    </xsl:function>

    <xsl:function name="cmake:find_re_core_library">
        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_library('re_core', 'RE_CORE_LIBRARIES', cmake:get_re_path('lib'))" />
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
            <xsl:when test="$middleware_lc = 'proto' or $middleware_lc = 'rti' or $middleware = 'ospl'">
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

    <xsl:function name="cmake:get_aggregates_middleware_file_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_label" select="lower-case(graphml:get_label($aggregate))" />
        <xsl:variable name="middleware_extension" select="cmake:get_middleware_extension($middleware)" />

        <xsl:value-of select="o:join_list(($aggregate_label, $middleware_extension), '.')" />
    </xsl:function>



    <xsl:function name="cmake:get_aggregates_middleware_file_path" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="file_path" select="cdit:get_aggregates_path($aggregate)" />
        <xsl:variable name="file_name" select="cmake:get_aggregates_middleware_file_name($aggregate, $middleware)" />
        
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
        <xsl:variable name="include_directory" select="upper-case(concat($middleware_package, '_INCLUDE_DIRS'))" />

        <xsl:if test="$middleware_package != ''">
            <xsl:value-of select="cmake:target_include_directories($target, cmake:wrap_variable($include_directory), $tab)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cmake:target_link_middleware_libraries">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="middleware_package" select="cmake:get_middleware_package($middleware)" />
        <xsl:variable name="include_directory" select="upper-case(concat($middleware_package, '_LIBRARIES'))" />

        <xsl:if test="$middleware_package != ''">
            <xsl:value-of select="cmake:target_link_libraries($target, cmake:wrap_variable($include_directory), $tab)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cmake:generate_middleware_compiler">
        <xsl:param name="file" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="middleware_extension" select="cmake:get_middleware_extension($middleware)" />
        <xsl:variable name="middleware_lc" select="lower-case($middleware)" />
        <xsl:variable name="source" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="header" select="cmake:get_middleware_generated_header_var($middleware)" />

        <xsl:variable name="middleware_compiler">
            <xsl:choose>
                <xsl:when test="$middleware_lc = 'rti'">
                    <xsl:value-of select="'RTI_GENERATE_CPP'" />
                </xsl:when>
                <xsl:when test="$middleware_lc = 'ospl'">
                    <xsl:value-of select="'OSPL_GENERATE_CPP'" />
                </xsl:when>
                <xsl:when test="$middleware_lc = 'proto'">
                    <xsl:value-of select="'protobuf_generate_cpp'" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cmake:comment(('UNKNOWN MIDDLEWARE', $middleware_lc), 0)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        
        <xsl:value-of select="cmake:comment(('Run the', $middleware, 'Compiler over the', o:wrap_angle($middleware_extension), 'files'), 0)" />
        <xsl:variable name="compiler_args" select="o:join_list(($source, $header, $file), ' ')" />
        <xsl:value-of select="concat($middleware_compiler, o:wrap_bracket($compiler_args), o:nl(1))" />
    </xsl:function>

    <xsl:function name="cmake:get_aggregates_middleware_shared_library_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:value-of select="lower-case(o:join_list(($middleware, $aggregate_namespace, $aggregate_label, 'lib'), '_'))" />
    </xsl:function>


    <xsl:function name="cmake:get_aggregates_middleware_module_library_name" as="xs:string">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:value-of select="lower-case(o:join_list(($middleware, $aggregate_namespace, $aggregate_label), '_'))" />
    </xsl:function>

  
    

    

</xsl:stylesheet>