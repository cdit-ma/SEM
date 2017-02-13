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
        <xsl:value-of select="concat('// ', $text, o:nl())" />
    </xsl:function>

    <xsl:function name="o:cpp_func_decl">
        <xsl:param name="return_type" as="xs:string"  />
        <xsl:param name="name" as="xs:string"  />
        <xsl:param name="params" as="xs:string"  />
        <xsl:value-of select="concat(o:t(2), $return_type, ' ', $name, '(', $params, ');', o:nl())" />
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

    <xsl:function name="o:cpp_base_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has get_ and same case -->
        <xsl:value-of select="concat('get_', $var_name)" />
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
                <xsl:value-of select="concat('//Unknown Type: ', o:quote_wrap($type))" />
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
                <xsl:value-of select="concat('//Unknown Type: ', o:quote_wrap($type))" />
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

     <xsl:function name="cdit:get_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:for-each select="$root">
            <xsl:variable name="kind_id" select="cdit:get_key_id(., 'kind')" />        
            <xsl:sequence select="$root//gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:for-each>
    </xsl:function>


    
</xsl:stylesheet>