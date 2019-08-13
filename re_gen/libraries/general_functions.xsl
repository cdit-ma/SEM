<!--
    A set of general purpose XSLT2.0 Functions
-->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:o="http://github.com/cdit-ma/re_gen/o"
    >

    <!--
        Gets n number of new line characters
    -->
    <xsl:function name="o:nl" as="xs:string">
        <xsl:param name="n" as="xs:integer"/>

        <xsl:variable name="text">
            <xsl:for-each select="1 to $n">
                <xsl:text>&#xa;</xsl:text>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$text" />
    </xsl:function>

    <!--
        Gets n number of tab characters
    -->
    <xsl:function name="o:t" as="xs:string">
        <xsl:param name="n" as="xs:integer"/>

        <xsl:variable name="text">
            <xsl:for-each select="1 to $n">
                <xsl:text>&#x9;</xsl:text>
            </xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$text" />
    </xsl:function>

    <!--
        Gets the greater than symbol (>)
    -->
    <xsl:function name="o:gt" as="xs:string">
        <xsl:text>&gt;</xsl:text>
    </xsl:function>

    <!--
        Gets the greater than symbol (<)
    -->
    <xsl:function name="o:lt" as="xs:string">
        <xsl:text>&lt;</xsl:text>
    </xsl:function>	

    <!--
        Gets the ampersand symbol (&)
    -->
    <xsl:function name="o:and" as="xs:string">
        <xsl:text>&amp;</xsl:text>
    </xsl:function>

    <!--
        Gets the At symbol (@)
    -->
    <xsl:function name="o:at" as="xs:string">
        <xsl:text>@</xsl:text>
    </xsl:function>

    <!--
        Gets the single quote symbol (')
    -->
    <xsl:function name="o:quote" as="xs:string">
        <xsl:text>'</xsl:text>
    </xsl:function>

    <!--
        Gets the double quote symbol (") 
    -->
    <xsl:function name="o:dblquote" as="xs:string">
        <xsl:text>"</xsl:text>
    </xsl:function>

    <!--
        Wraps the provided string in the set of symbols provided
        ${start}${str}${end}
    -->
    <xsl:function name="o:wrap" as="xs:string">
        <xsl:param name="start" as="xs:string"/>
        <xsl:param name="str" as="xs:string"/>
        <xsl:param name="end" as="xs:string"/>
        <xsl:value-of select="concat($start, $str, $end)" />
    </xsl:function>	

    <!--
        Wraps the provided string in single quotes
        ie. '${str}'
    -->
    <xsl:function name="o:wrap_quote" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:quote(), $str, o:quote())" />
    </xsl:function>

    <!--
        Wraps the provided string in double quotes
        ie. "${str}"
    -->
    <xsl:function name="o:wrap_dblquote" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:dblquote(), $str, o:dblquote())" />
    </xsl:function>

    <!--
        Wraps the provided string in brackets
        ie. (${str})
    -->
    <xsl:function name="o:wrap_bracket" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('(', $str, ')')" />
    </xsl:function>

    <!--
        Wraps the provided string in a angle brackets
        ie. <${str}>
    -->
    <xsl:function name="o:wrap_angle" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap(o:lt(), $str, o:gt())" />
    </xsl:function>	

    <!--
        Wraps the provided string in a square brackets
        ie. [${str}]
    -->
    <xsl:function name="o:wrap_square">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('[', $str, ']')" />
    </xsl:function>

    <!--
        Wraps the provided string in a curly brackets
        {${str}}
    -->
    <xsl:function name="o:wrap_curly" as="xs:string">
        <xsl:param name="str" as="xs:string"/>
        <xsl:value-of select="o:wrap('{', $str, '}')" />
    </xsl:function>

    <!--
        Captilizes the first letter of the string provided
        ie. 'hello' -> 'Hello'
    -->
    <xsl:function name="o:title_case" as="xs:string?">
        <xsl:param name="str" as="xs:string?"  />

        <xsl:if test="string-length($str) > 0">
            <xsl:value-of select="concat(upper-case(substring($str, 1, 1)), substring($str, 2))" />
        </xsl:if>
    </xsl:function>

    <!--
        uncaptilizes the first letter of the string provided
        ie. 'Gello' -> 'hello'
    -->
    <xsl:function name="o:untitle_case" as="xs:string">
        <xsl:param name="str" as="xs:string"  />

        <xsl:if test="string-length($str) > 0">
            <xsl:value-of select="concat(lower-case(substring($str, 1, 1)), substring($str, 2))" />
        </xsl:if>
    </xsl:function>

    <!--
        Concatenates the path provided, and produces a xsl:message saying that the file is to be written.
        Useful for showing the written files from a transform.
    -->
    <xsl:function name="o:write_file" as="xs:string">
        <xsl:param name="file_path" as="xs:string*" />

        <xsl:variable name="resolved_file_path" select="o:join_paths($file_path)" />
        <xsl:if test="$debug_mode">
            <xsl:message>Created File: <xsl:value-of select="$resolved_file_path" /></xsl:message>
        </xsl:if>
        <xsl:value-of select="$resolved_file_path" />
    </xsl:function>

    <!--
        Joins the list of strings provided, seperating them with the provided token.
        Will remove any blanks strings from the list
    -->
    <xsl:function name="o:join_list" as="xs:string">
        <xsl:param name="list" as="xs:string*"/>
        <xsl:param name="token" as="xs:string" />
        
        <xsl:variable name="pruned_list" as="xs:string*" select="o:trim_list($list)" />
        <xsl:value-of select="string-join($pruned_list, $token)" />
    </xsl:function>

    <!--
        Removes any empty strings in a list of strings
    -->
    <xsl:function name="o:trim_list" as="xs:string*">
        <xsl:param name="list" as="xs:string*"/>
        
        <xsl:for-each select="$list">
            <xsl:if test=". != ''">
                <xsl:sequence select="." />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <!--
        Joins the list of paths provided, seperating them the '/' character
    -->
    <xsl:function name="o:join_paths" as="xs:string">
        <xsl:param name="list" as="xs:string*"/>
        <xsl:value-of select="o:join_list($list, '/')" />
    </xsl:function>

    <!--
        Prints a Warning message
    -->
    <xsl:function name="o:warning">
        <xsl:param name="message" as="xs:string*"/>
        <xsl:message>Warning: <xsl:value-of select="o:join_list($message, ' ')" /></xsl:message>
    </xsl:function>

    <!--
        Prints a message to standard out
    -->
    <xsl:function name="o:message">
        <xsl:param name="message" as="xs:string*"/>
        <xsl:message><xsl:value-of select="o:join_list($message, ' ')" /></xsl:message>
    </xsl:function>

    <!--
        Escapes any regex meta characters
    -->
    <xsl:function name="o:escape_regex">
        <xsl:param name="str" as="xs:string"/>
        <xsl:variable name="a" select="replace($str, '$', '\$', 'q')" />
        <xsl:variable name="b" select="replace($a, '^', '\^', 'q')" />
        <xsl:variable name="c" select="replace($b, '.', '\.', 'q')" />
        <xsl:variable name="d" select="replace($c, '(', '\(', 'q')" />
        <xsl:variable name="e" select="replace($d, ')', '\)', 'q')" />
        <xsl:value-of select="$e" />
    </xsl:function>

    <!--
        Prints a message to standard out
    -->
    <xsl:function name="o:string_in_list_count" as="xs:integer">
        <xsl:param name="str" as="xs:string"/>
        <xsl:param name="string_list" as="xs:string*"/>
        <xsl:param name="case_insensitive" as="xs:boolean"/>

        <xsl:variable name="escaped_str" select="concat('^', o:escape_regex($str), '$')"/>
        <xsl:value-of select="count($string_list[matches(., $escaped_str, if($case_insensitive) then 'i' else '')])" />
    </xsl:function>

    <!--
        Removes duplicate nodes from a list
    -->
    <xsl:function name="o:remove_duplicates">
        <xsl:param name="elements" />

        <xsl:for-each-group select="$elements" group-by="generate-id(.)">
            <xsl:sequence select="." />
        </xsl:for-each-group>
    </xsl:function>
</xsl:stylesheet>