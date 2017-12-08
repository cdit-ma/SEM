<!-- General Functions for XSL output -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/o"
    >

    <!--
        adds a line end
    -->
    <xsl:function name="o:nl" as="xs:string">
        <xsl:param name ="count"/>

        <xsl:variable name="total">
             <xsl:for-each select="1 to $count">
                <xsl:text>&#xa;</xsl:text>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$total" />
    </xsl:function>

    <!--
        adds a tab
    -->
    <xsl:function name="o:t" as="xs:string">
        <xsl:param name ="count"/>

        <xsl:variable name="total">
             <xsl:for-each select="1 to $count">
                <xsl:text>&#x9;</xsl:text>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$total" />
    </xsl:function>

    <!--
        adds a greater than symbol (>)
    -->
    <xsl:function name="o:gt" as="xs:string">
        <xsl:text>&gt;</xsl:text>
    </xsl:function>

    <!--
        adds a less than symbol (<)
    -->
    <xsl:function name="o:lt" as="xs:string">
        <xsl:text>&lt;</xsl:text>
    </xsl:function>	

    <!--
        adds an ampersand symbol (&)
    -->
    <xsl:function name="o:and" as="xs:string">
        <xsl:text>&amp;</xsl:text>
    </xsl:function>

    <!--
        adds an at symbol (@)
    -->
    <xsl:function name="o:at" as="xs:string">
        <xsl:text>@</xsl:text>
    </xsl:function>

    <!--
        adds a single quote symbol (')
    -->
    <xsl:function name="o:quote" as="xs:string">
        <xsl:text>'</xsl:text>
    </xsl:function>

    <!--
        adds a double quote symbol (")
    -->
    <xsl:function name="o:dblquote" as="xs:string">
        <xsl:text>"</xsl:text>
    </xsl:function>

    <!--
        wraps a string in the provided string
        ${start}${str}${end}
    -->
    <xsl:function name="o:wrap" as="xs:string">
        <xsl:param name="start" />
        <xsl:param name="str" as="xs:string"/>
        <xsl:param name="end" />
        <xsl:value-of select="concat($start, $str, $end)" />
    </xsl:function>	

    <!--
        Wraps a string in a single quote symbols (')
        '${str}'
    -->
    <xsl:function name="o:wrap_quote" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:quote(), $str, o:quote())" />
    </xsl:function>

    <!--
        Wraps a string in a double quote symbols (")
        "${str}"
    -->
    <xsl:function name="o:wrap_dblquote" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:dblquote(), $str, o:dblquote())" />
    </xsl:function>

    <!--
        Wraps a string in brackets
        (${str})
    -->
    <xsl:function name="o:wrap_bracket" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('(', $str, ')')" />
    </xsl:function>

    <!--
        Wraps a string in a angle brackets
        <${str}>
    -->
    <xsl:function name="o:wrap_angle" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:lt(), $str, o:gt())" />
    </xsl:function>	

    <!--
        Wraps a string in square brackets
        [${str}]
    -->
    <xsl:function name="o:wrap_square">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('[', $str, ']')" />
    </xsl:function>

    <!--
        Wraps a string in curly braces
        {${str}}
    -->
    <xsl:function name="o:wrap_curly" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('{', $str, '}')" />
    </xsl:function>

    <!--
        Camel cases a string
    -->
    <xsl:function name="o:title_case" as="xs:string">
        <xsl:param name="str" as="xs:string"  />

        <xsl:if test="string-length($str) > 0">
            <xsl:value-of select="concat(upper-case(substring($str, 1, 1)), substring($str, 2))" />
        </xsl:if>
    </xsl:function>

    <!--
        Produces a message to say that this file will be produced
    -->
    <xsl:function name="o:write_file" as="xs:string">
        <xsl:param name="file_path" />
        <xsl:message>Created File: <xsl:value-of select="$file_path" /></xsl:message>
        <xsl:value-of select="$file_path" />
    </xsl:function>

    <!--
        List joins
    -->
    <xsl:function name="o:join_list" as="xs:string">
        <xsl:param name="list" as="xs:string*"/>
        <xsl:param name="token" as="xs:string" />
        
        <xsl:variable name="pruned_list" as="xs:string*">
            <xsl:for-each select="$list">
                <xsl:if test=". != ''">
                    <xsl:value-of select="." />
                </xsl:if>
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="string-join($pruned_list, $token)" />
    </xsl:function>

    <!--
        List joins
    -->
    <xsl:function name="o:join_paths" as="xs:string">
        <xsl:param name="list" as="xs:string*"/>
        <xsl:value-of select="o:join_list($list, '/')" />
    </xsl:function>

    <!--
        prints a warning
    -->
    <xsl:function name="o:warning">
        <xsl:param name="message" as="xs:string*"/>
        <xsl:message>Warning: <xsl:value-of select="$message" /></xsl:message>
    </xsl:function>
</xsl:stylesheet>