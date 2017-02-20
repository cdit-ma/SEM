<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    >

    <!-- find the key for specific attribute,  -->
    <xsl:template name="find_key">
        <xsl:param name="key_name" />
        <xsl:param name="default_id" />

        <xsl:variable name="found" select="/gml:graphml/gml:key[@attr.name=$key_name]" />
        
        <xsl:choose>
            <xsl:when test="not($found)">
                <xsl:value-of select="$default_id"/>
            </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$found/@id"/>
        </xsl:otherwise>
        </xsl:choose>
    </xsl:template>	


    <!-- find the key for specific attribute, -->
    <xsl:template name="get_key_value">
        <xsl:param name="key_name" />
        <xsl:param name="default_id" />
        
        <xsl:variable name="key_id">
            <xsl:call-template name="find_key">
                <xsl:with-param name="key_name" select="$key_name" />
                <xsl:with-param name="default_id" select="$key_name" />
            </xsl:call-template>	
        </xsl:variable>
        
        <xsl:value-of select="./gml:data[@key=$key_id]"/>
    </xsl:template>	

    <xsl:function name="o:nl">
        <xsl:text>&#xa;</xsl:text>
    </xsl:function>	

    <xsl:function name="o:t">
        <xsl:param name ="count"/>

        <xsl:variable name="total">
             <xsl:for-each select="1 to $count"><xsl:text>&#x9;</xsl:text></xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$total" />
    </xsl:function>	

    <xsl:function name="o:fp">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <xsl:function name="o:gt">
        <xsl:text>&gt;</xsl:text>
    </xsl:function>

    <xsl:function name="o:lt">
        <xsl:text>&lt;</xsl:text>
    </xsl:function>	

    <xsl:function name="o:and">
        <xsl:text>&amp;</xsl:text>
    </xsl:function>
    
    <xsl:function name="o:quote">
        <xsl:text>'</xsl:text>
    </xsl:function>	
    
    <xsl:function name="o:dblquote">
        <xsl:text>"</xsl:text>
    </xsl:function>	

    <xsl:function name="o:quote_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:quote(), $str, o:quote())" />
    </xsl:function>	

    <xsl:function name="o:angle_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:lt(), $str, o:gt())" />
    </xsl:function>	

     <xsl:function name="o:bracket_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat('(', $str, ')')" />
    </xsl:function>	

    <xsl:function name="o:dblquote_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:dblquote(), $str, o:dblquote())" />
    </xsl:function>	
    
    <xsl:function name="o:camel_case">
        <xsl:param name="str" as="xs:string"  />
        <xsl:value-of select="concat(upper-case(substring($str,1,1)), substring($str, 2))" />
    </xsl:function>
    
    <xsl:function name="o:lib_include">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:value-of select="concat('#include ', o:angle_wrap($lib), o:nl())" />
    </xsl:function>	

    <xsl:function name="o:local_include">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:value-of select="concat('#include ', o:dblquote_wrap($lib), o:nl())" />
    </xsl:function>

    <xsl:function name="o:cpp_comment">
        <xsl:param name="text" as="xs:string"  />

        <xsl:value-of select="o:tabbed_cpp_comment($text, 0)" />
    </xsl:function>

    <xsl:function name="o:tabbed_cpp_comment">
        <xsl:param name="text" as="xs:string"  />
        <xsl:param name="tab" as="xs:integer"  />
        <xsl:value-of select="concat(o:t($tab), '// ', $text, o:nl())" />
    </xsl:function>


    <xsl:function name="o:cpp_func_def">
        <xsl:param name="return_type" as="xs:string"  />
        <xsl:param name="namespace" as="xs:string"  />
        <xsl:param name="name" as="xs:string"  />
        <xsl:param name="params" as="xs:string"  />

        <xsl:value-of select="concat($return_type, ' ', if($namespace !='') then concat($namespace, '::') else '', $name, '(', $params, ')')" />
    </xsl:function>

    <xsl:function name="o:cpp_var_decl">
        <xsl:param name="type" as="xs:string"  />
        <xsl:param name="name" as="xs:string"  />
        <xsl:value-of select="concat(o:t(2), $type, ' ', $name, ';', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_find_library">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:param name="lib_vars" as="xs:string"  />
        <xsl:param name="lib_path" as="xs:string"  />

        
        <xsl:value-of select="o:cmake_comment(concat('Find library ', $lib))" />
        <xsl:value-of select="concat('find_library(', $lib_vars, ' ', $lib, ' ', o:dblquote_wrap($lib_path), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_find_package">
        <xsl:param name="lib" as="xs:string"  />

        <xsl:value-of select="o:cmake_comment(concat('Find package ', $lib))" />
        <xsl:value-of select="concat('find_package(', $lib, ' REQUIRED)', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_set_env">
        <xsl:param name="var" as="xs:string"  />
        <xsl:param name="val" as="xs:string"  />
        <xsl:value-of select="concat('set(', $var, ' ',  o:dblquote_wrap($val), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_set_proj_name">
        <xsl:param name="name" as="xs:string"  />

        <xsl:value-of select="o:cmake_set_env('PROJ_NAME', $name)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_include_dir">
        <xsl:param name="dir" as="xs:string"  />
        <xsl:value-of select="concat('include_directories(', o:dblquote_wrap($dir), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_comment">
        <xsl:param name="text" as="xs:string"  />
        <xsl:value-of select="concat('# ', $text, o:nl())" />
    </xsl:function>

    <xsl:function name="o:define_guard">
        <xsl:param name="lib_name" as="xs:string"  />
        
        <xsl:value-of select="concat('#ifndef ', $lib_name, o:nl())" />
        <xsl:value-of select="concat('#define ', $lib_name, o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    <xsl:function name="o:define_guard_end">
        <xsl:param name="lib_name" as="xs:string" />
        <xsl:value-of select="concat(o:nl(), '#endif //', $lib_name, o:nl())" />
    </xsl:function>


    <xsl:function name="o:cpp_proto_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has no get_ but uses lower-case -->
        <xsl:value-of select="lower-case($var_name)" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('set_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_mw_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS uses exact case -->
                <xsl:value-of select="$var_name" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses exact case -->
                <xsl:value-of select="$var_name" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Protobuf uses lowercase -->
                <xsl:value-of select="lower-case($var_name)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
        
    </xsl:function>

    

    <xsl:function name="o:cpp_mw_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS doesn't use get/set functions and return references -->
                <!-- ie val() = val -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap($value))" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses get/set functions -->
                <!-- ie set_val(val) -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''), ' = ', $value)" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Base uses get/set functions -->
                <xsl:value-of select="concat('set_', $mw_name, o:bracket_wrap($value))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:cpp_mw_get_vector_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="index" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
             <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Protobuf uses $var_name($index) -->
                <!-- ie val(i) -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap($index))" />
            </xsl:when>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl' or $middleware = 'base'">
                <!-- DDS/Base uses $var_name()[$index] -->
                <!-- ie val()[i] -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''), '[', $index, ']')" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!-- Function to determine if a cast is needed to avoid ambiguous call to setter functions-->
    <xsl:function name="o:member_requires_cast">
        <xsl:param name="member_type" as="xs:string" />
        <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
        <xsl:value-of select="contains(lower-case($member_type), 'integer') = true()" />
    </xsl:function>

    <xsl:function name="o:process_member">
        <xsl:param name="member_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="member_label" select="cdit:get_key_value($member_root, 'label')" />
        <xsl:variable name="member_type" select="cdit:get_key_value($member_root, 'type')" />
        
        <!-- Check if we need to cast this value -->
        <xsl:variable name="cast" select="if(o:member_requires_cast($member_type) = true()) then o:bracket_wrap(cdit:get_cpp_type($member_type)) else ''" />
        
        <!-- Get the value; Using the base middlewares getter -->
        <xsl:variable name="value" select="concat($cast, $in_var, o:fp(), o:cpp_mw_get_func($member_label, $src_mw))" />

        <!-- Set the value; using the appropriate middlewares setter -->
        <xsl:value-of select="concat(o:t(1), $out_var, o:fp(), o:cpp_mw_set_func($member_label, $dst_mw, $value), ';', o:nl())" />
    </xsl:function>

    <xsl:function name="o:process_aggregate">
        <xsl:param name="aggregate_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_type" select="cdit:get_key_value($aggregate_root, 'type')" />

        <xsl:variable name="src_var" select="concat('src_', lower-case($aggregate_label), '_')" />
        <xsl:variable name="dst_var" select="concat('dst_', lower-case($aggregate_label), '_')" />

        <xsl:variable name="get_value" select="concat($in_var, o:fp(), o:cpp_mw_get_func($aggregate_label, $src_mw))" />
        
        <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($aggregate_type)), 1)" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $src_var, ' = ', $get_value, ';', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $dst_var, ' = ', $namespace, '::translate(',o:and(), $src_var, ');', o:nl())" />
                
        <xsl:value-of select="concat(o:t(1), 'if(', $dst_var, '){', o:nl())" />
            <xsl:choose>
                <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                    <!-- Protobuf have to use a swap function from the newly added element -->
                    <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), 'set_allocated_', o:cpp_mw_func($aggregate_label, $dst_mw), '(', $dst_var, ');', o:nl())" />
                </xsl:when>
                <xsl:otherwise>
                    <!-- Complex vectors in other middlewares use indexed insertion using a dereferenced pointer -->
                    <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), o:cpp_mw_set_func($aggregate_label, $dst_mw, concat('*', $dst_var)), ';', o:nl())" />
                </xsl:otherwise>
            </xsl:choose>
            <!-- Free Memory From Translate -->
            <xsl:value-of select="concat(o:t(2), 'delete ', $dst_var, ';', o:nl())" />
            <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
    </xsl:function>

    <xsl:function name="o:get_translate_cpp">
        <xsl:param name="members" />
        <xsl:param name="vectors" />
        <xsl:param name="aggregates" />
        <xsl:param name="src_type" as="xs:string" />
        <xsl:param name="dst_type" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="in_var" select="'src'" />
        <xsl:variable name="out_var" select="'dst_'" />

        <xsl:value-of select="concat($dst_type, '* ', $namespace, '::translate(const ', $src_type , ' *', $in_var, '){', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $out_var, ' = new ', $dst_type, '();', o:nl())" />
        
        <xsl:for-each select="$members">
            <xsl:value-of select="o:process_member(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <xsl:for-each select="$vectors">
            <xsl:value-of select="o:process_vector(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <xsl:for-each select="$aggregates">
            <xsl:value-of select="o:process_aggregate(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <!-- Return the Translated object -->
        <xsl:value-of select="concat(o:t(1), 'return ', $out_var, ';', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    <xsl:function name="o:process_vector">
        <xsl:param name="vector_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <!-- Get the Middleware which is used for the namespace functions -->
        <xsl:variable name="translate_mw" select="if(lower-case($src_mw) = 'base') then $dst_mw else $src_mw" />

        <!-- Get the appropriate fields from the vector -->
        <xsl:variable name="vector_label" select="cdit:get_key_value($vector_root, 'label')" />
        <xsl:variable name="vector_type" select="cdit:get_key_value($vector_root, 'type')" />
        <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type($vector_root)" />
        <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex($vector_root)" />
        <!-- lower-case($vector_label) -->
        <xsl:variable name="src_var" select="concat('src_', lower-case($vector_label), '_i_')" />
        <xsl:variable name="dst_var" select="concat('dst_', lower-case($vector_label), '_i_')" />

        <xsl:variable name="src_mw_uses_pb" select=" o:middleware_uses_protobuf($src_mw)" />
        <xsl:variable name="dst_mw_uses_pb" select=" o:middleware_uses_protobuf($dst_mw)" />

        <!-- Opening Brace -->
        <xsl:value-of select="concat(o:t(1), '{', o:nl())" />
        <xsl:value-of select="o:tabbed_cpp_comment(concat('o:process_vector() ', $vector_label, o:angle_wrap($vector_cpp_type)), 2)" />

        <!-- Get a copy of the vector-->
        <xsl:value-of select="o:tabbed_cpp_comment('Copy Vector', 2)" />
        <xsl:variable name="vector_copy" select="concat($vector_label, '_')" />
        <xsl:value-of select="concat(o:t(2), 'auto ', $vector_copy, ' = ', $in_var, o:fp(), o:cpp_mw_get_func($vector_label, $src_mw), ';', o:nl())" />

        <!-- Get the source vector size -->
        <xsl:value-of select="o:tabbed_cpp_comment('Get the size of the source vector', 2)" />
        <xsl:value-of select="concat(o:t(2), 'auto size_ = ', $vector_copy, '.size();', o:nl())" />

        <xsl:if test="$dst_mw_uses_pb = false()">
            <!-- For non protobuf vectors, resize the target vector -->
            <xsl:value-of select="o:tabbed_cpp_comment('Resize the destination vector', 2)" />
            <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '().resize(size_);', o:nl())" />
        </xsl:if>

        <!-- Define for loop -->
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:tabbed_cpp_comment('Itterate through source vector', 2)" />
        <xsl:value-of select="concat(o:t(2), 'for(int i = 0; i ', o:lt(), ' size_; i++){', o:nl())" />

        

        <xsl:variable name="get_src_val" select="concat($vector_copy, '[i]')" />
        
        <!-- Set the target vector element -->
        <xsl:choose>
            <xsl:when test="$is_vector_complex">
                <!-- Complex types -->
                <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($vector_cpp_type)), 3)" />
                <xsl:value-of select="concat(o:t(3), 'auto ', $dst_var, ' = ', $namespace, '::translate(',o:and(),  o:bracket_wrap($get_src_val), ');', o:nl())" />
                
                <xsl:value-of select="concat(o:t(3), 'if(', $dst_var, '){', o:nl())" />
                <xsl:choose>
                    <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                        <xsl:variable name="new_element" select="concat($dst_var, 'pb_')" />
                        <xsl:value-of select="o:tabbed_cpp_comment('Add element to the destination vector', 4)" />
                        <xsl:value-of select="concat(o:t(4), 'auto ', $new_element, ' = ', $out_var, o:fp(), o:cpp_mw_func(concat('add_', $vector_label), $dst_mw),'();', o:nl())" />
                        <xsl:value-of select="o:tabbed_cpp_comment('Swap the contents into the destination vector', 4)" />
                        <!-- Complex vectors in protobuf have to use a swap function from the newly added element -->
                        <xsl:value-of select="concat(o:t(4), $new_element, o:fp(), 'Swap(', $dst_var, ');', o:nl())" />
                    </xsl:when>
                    <xsl:otherwise>
                        <!-- Complex vectors in other middlewares use indexed insertion using a dereferenced pointer -->
                        <xsl:value-of select="concat(o:t(4), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '()[i] = *', $dst_var, ';', o:nl())" />
                    </xsl:otherwise>
                </xsl:choose>
                <!-- Free Memory From Translate -->
                <xsl:value-of select="concat(o:t(4), 'delete ', $dst_var, ';', o:nl())" />
                <xsl:value-of select="concat(o:t(3), '}', o:nl())" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                        <!-- Primitive vectors in protobuf use an add_ function with value as a parameter -->
                        <xsl:value-of select="concat(o:t(3), $out_var, o:fp(), o:cpp_mw_func(concat('add_', $vector_label), $src_mw),'(', $get_src_val, ');', o:nl())" />
                    </xsl:when>
                    <xsl:otherwise>
                        <!-- Primitive vectors in other middlewares simply use indexed insertion -->
                        <xsl:value-of select="concat(o:t(3), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '()[i] = ', $get_src_val, ';', o:nl())" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:value-of select="concat(o:t(2), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
    </xsl:function>

 
    <xsl:function name="o:cpp_mw_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS doesn't use get/set functions -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses get/set functions-->
                <xsl:value-of select="concat('get_', $mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="o:cpp_set_translated_val">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="var_type" as="xs:string" />
        <xsl:param name="mw" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="mw_var_name" select="concat($var_name, $mw, '_')" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($var_type)), $tab)" />
        <xsl:value-of select="concat(o:t($tab), '{', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), 'auto ', $mw_var_name, ' = ', $mw, '::translate(', o:and(), $var_name ,');', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), 'if(', $mw_var_name, '){', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+2), 'out_', o:fp(), o:cpp_mw_get_func($var_name, $mw), '()[i] = ', '*', $mw_var_name, ';', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+2), 'delete ', $mw_var_name, ';', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), '}', o:nl())" />
        <xsl:value-of select="concat(o:t($tab), '}', o:nl())" />
        
    </xsl:function>


    <xsl:function name="o:cpp_dds_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has no set prefix, and is in camel-case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_dds_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has no get prefix, and is in camel-case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_set_complex_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('set_allocated_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_release_complex_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('release_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_add_vector">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:value-of select="(concat('add_', lower-case($var_name), '(', $value, ')'))" />
    </xsl:function>

    <xsl:function name="o:cpp_base_add_vector">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:value-of select="concat($var_name, '().push_back(', $value, ')')" />
    </xsl:function>




     <xsl:function name="o:construct_rx">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        <xsl:value-of select="'EventPort* ConstructRx(std::string port_name, Component* component){'" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="concat(o:t(1), 'EventPort* p = 0;', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(component){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'auto fn = component', o:fp(), 'GetCallback(port_name);', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'if(fn){', o:nl())" />
        <xsl:value-of select="concat(o:t(3), 'p = new ', $middleware, '::InEventPort', o:angle_wrap(concat($base_type, ', ', $mw_type)),'(component, port_name, fn);', o:nl())" />
        <xsl:value-of select="concat(o:t(2), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return p;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>

    <xsl:function name="o:construct_tx">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        <xsl:value-of select="'EventPort* ConstructTx(std::string port_name, Component* component){'" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="concat(o:t(1), 'return new ', $middleware, '::OutEventPort', o:angle_wrap(concat($base_type, ', ', $mw_type)),'(component, port_name);', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>

    <xsl:function name="cdit:middleware_uses_protobuf" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string" />

        <xsl:value-of select="$middleware='qpid' or $middleware='zmq' or $middleware='proto'" />
    </xsl:function>


    <xsl:function name="cdit:get_middleware_file_header" as="xs:string">
        <xsl:param name="aggregate" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti'">
                <xsl:value-of select="concat($aggregate, '.hpp')" />
            </xsl:when>
            <xsl:when test="$middleware = 'ospl'">
                <xsl:value-of select="concat($aggregate, '_DCPS.hpp')" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="''" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middlware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:get_libport_export">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        
        <xsl:variable name="using_pb" select ="cdit:middleware_uses_protobuf($middleware)" />

        <xsl:variable name="aggregate_label" select ="cdit:get_key_value($aggregate, 'label')" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <xsl:variable name="mw_header_file" select="cdit:get_middleware_file_header($aggregate_label_lc, $middleware)" />

        
        <!-- Include libportexports.h -->            
        <xsl:value-of select="o:lib_include('core/libportexports.h')" />
        <xsl:value-of select="o:nl()" />

        <!-- Include Convert functions -->
        <xsl:choose>
            <xsl:when test="$using_pb">
                <xsl:value-of select="o:cpp_comment('Include the proto convert functions for the port type')" />
                <xsl:value-of select="o:local_include(concat('../../proto/', $aggregate_label_lc, '/convert.h'))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:local_include('convert.h')" />
            </xsl:otherwise>
        </xsl:choose>

        
        <xsl:if test="$using_pb = false()" >
            <xsl:value-of select="o:local_include($mw_header_file)" />
        </xsl:if>


        
        <xsl:value-of select="o:nl()" />
        
        <!-- Include Templated EventPort classes -->
        <xsl:value-of select="o:cpp_comment(concat('Include the ', $middleware, ' specific template ports'))" />
        <xsl:value-of select="o:lib_include(concat('middleware/', $middleware, '/ineventport.hpp'))" />
        <xsl:value-of select="o:lib_include(concat('middleware/', $middleware, '/outeventport.hpp'))" />
        <xsl:value-of select="o:nl()" />
        
        <!-- Construct RX -->
        <xsl:value-of select="o:construct_rx($middleware, $base_type, $mw_type)" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:construct_tx($middleware, $base_type, $mw_type)" />
    </xsl:function>
    

    

    <xsl:function name="o:cpp_base_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has get_ and same case -->
        <xsl:value-of select="concat('get_', $var_name)" />
    </xsl:function>

    <xsl:function name="o:cpp_base_get_ptr_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has get_ and same case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_base_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has set_ and same case -->
        <xsl:value-of select="concat('set_', $var_name)" />
    </xsl:function>

    <xsl:function name="o:namespace">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:value-of select="concat('namespace ', $namespace, '{', o:nl())" />
    </xsl:function>

    <xsl:function name="o:forward_declare_class">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="concat(o:namespace($namespace), o:t(1))" />
        </xsl:if>

        <xsl:value-of select="concat('class ', $class_name, ';', o:nl())" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="concat('};', o:nl())" />
        </xsl:if>
    </xsl:function>


    <xsl:function name="o:declare_variable_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />

        <xsl:variable name="variable_ptr_type" select="concat($variable_type, o:and())" />

        
        <!-- Public Declarations -->
        <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
        <!-- Copy Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, ' val')), ';', o:nl())" />
        <!-- Pointer Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, '* val')), ';', o:nl())" />
        
        <!-- Copy Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_type, '', o:cpp_base_get_func($variable_name), ''), ' const;', o:nl())" />
        <!-- Inplace Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_ptr_type, '', $variable_name, ''), ';', o:nl())" />
        
        <!-- Private Declarations -->
        <xsl:value-of select="concat(o:t(1), 'private:', o:nl())" />
        <!-- Variable -->
        <xsl:value-of select="o:cpp_var_decl($variable_type, concat($variable_name, '_'))" />
    </xsl:function>

    <xsl:function name="o:define_variable_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="variable_var" select="concat($variable_name, '_')" />
        <xsl:variable name="variable_ptr_type" select="concat($variable_type, o:and())" />

        <!-- Copy Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($variable_type, ' val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'this', o:fp(), $variable_var, ' = val;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Pointer Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($variable_type, '* val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(val){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'this', o:fp(), $variable_var, ' = *val;', o:nl())" />
        <xsl:value-of select="concat(o:t(1),'}', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Copy Getter -->
        <xsl:value-of select="o:cpp_func_def($variable_type, $class_name, o:cpp_base_get_func($variable_name), '')" />
        <xsl:value-of select="concat(' const {', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return this', o:fp(), $variable_var, ';', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Inplace Getter -->
        <xsl:value-of select="o:cpp_func_def($variable_ptr_type, $class_name, $variable_name, '')" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return this', o:fp(), $variable_name, '_;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>
   
                


    <xsl:function name="cdit:get_cpp_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'String'">
                <xsl:value-of select="'std::string'" />
            </xsl:when>
            <xsl:when test="$type = 'Boolean'">
                <xsl:value-of select="'bool'" />
            </xsl:when>
            <xsl:when test="$type = 'FloatNumber'">
                <xsl:value-of select="'float'" />
            </xsl:when>
            <xsl:when test="$type = 'DoubleNumber' or $type = 'LongDoubleNumber'">
                <xsl:value-of select="'double'" />
            </xsl:when>
            <xsl:when test="$type = 'LongInteger'">
                <xsl:value-of select="'int'" />
            </xsl:when>
            
            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_cpp2proto_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
            <xsl:when test="$type = 'int'">
                <xsl:value-of select="'int64'" />
            </xsl:when>
            <xsl:when test="$type = 'double' or $type = 'float' or $type = 'bool'">
                <xsl:value-of select="$type" />
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_cpp2dds_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
            <xsl:when test="$type = 'int'">
                <xsl:value-of select="'long'" />
            </xsl:when>
            <xsl:when test="$type = 'double' or $type = 'float' or $type = 'bool'">
                <xsl:value-of select="$type" />
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_key_id" as="xs:string">
        <xsl:param name="root"/>
        <xsl:param name="key_name" as="xs:string"/>
        
        <xsl:for-each select="$root">
            <xsl:value-of select="//gml:key[@attr.name = $key_name]/@id" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_key_value" as="xs:string">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:for-each select="$root">
            <xsl:variable name="key_id" select="cdit:get_key_id(., $key_name)" />        
            <xsl:value-of select="$root/gml:data[@key=$key_id]/text()" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:is_key_value_true" as="xs:boolean">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:value-of select="lower-case(cdit:get_key_value($root, $key_name)) = 'true'" />
    </xsl:function>

    

     <xsl:function name="cdit:get_child_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:for-each select="$root">
            <xsl:variable name="kind_id" select="cdit:get_key_id(., 'kind')" />        
            <xsl:sequence select="$root/gml:graph/gml:node/gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:for-each select="$root">
            <xsl:variable name="kind_id" select="cdit:get_key_id(., 'kind')" />        
            <xsl:sequence select="$root//gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_required_datatypes">
        <xsl:param name="vectors" as="element()*"/>
        <xsl:param name="aggregates" as="element()*"/>

        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_type" select="lower-case(cdit:get_key_value(., 'type'))" />
        
            <xsl:value-of select="$aggregate_type" />
        </xsl:for-each>

        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_type" select="lower-case(cdit:get_key_value(., 'type'))" />
        
            <xsl:value-of select="$aggregate_type" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:is_vector_complex" as="xs:boolean">
        <xsl:param name="vector" as="element()*"/>

        <xsl:variable name="first_child" select="$vector/gml:graph[1]/gml:node[1]" />
        <!-- Get the kind of the first child in the Vector Type -->
        <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
        
        <!-- Get the type of the vector -->
        <xsl:variable name="is_aggregate" select="$vector_child_kind = 'AggregateInstance' or $vector_child_kind = 'Aggregate'" />
        <xsl:value-of select="$is_aggregate" />
    </xsl:function>

    <xsl:function name="cdit:get_vector_type">
        <xsl:param name="vector" as="element()*"/>

        <xsl:variable name="first_child" select="$vector/gml:graph[1]/gml:node[1]" />
        <xsl:variable name="vector_label" select="cdit:get_key_value($vector, 'label')" />

        <!-- Get the Vector Type -->
        <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
        <xsl:variable name="vector_child_type" select="cdit:get_key_value($first_child, 'type')" />
        
        <xsl:choose>
            <xsl:when test="cdit:is_vector_complex($vector)">
                <xsl:value-of select="$vector_child_type"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:get_cpp_type($vector_child_type)"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cdit:get_aggregate_index">
        <xsl:param name="node" as="element()*"/>

        <xsl:variable name="index" select="count($node/preceding-sibling::gml:node) + 1" />
        <xsl:value-of select="$index" />
    </xsl:function>


    <xsl:function name="cdit:get_namespace">
        <xsl:param name="node" as="element()*"/>

        <!-- Follow Definition Edges -->
        <xsl:value-of select="'HelloWorld'" />
    </xsl:function>

    


    
</xsl:stylesheet>