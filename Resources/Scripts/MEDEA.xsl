<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:gml="http://graphml.graphdrawing.org/xmlns" xmlns:xalan="http://xml.apache.org/xalan" version="1.0">
<!--Implementers: please note that overriding process-prolog or process-root is 
    the preferred method for meta-stylesheets to use where possible. -->
<axsl:param name="archiveDirParameter"/>
<axsl:param name="archiveNameParameter"/>
<axsl:param name="fileNameParameter"/>
<axsl:param name="fileDirParameter"/>

<!--PHASES-->


<!--PROLOG-->
<axsl:output xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" method="xml" omit-xml-declaration="no" standalone="yes" indent="yes"/>

<!--KEYS-->
<axsl:key name="fileLabels" match="gml:node[gml:data/text()='File']" use="gml:data"/>
<axsl:key name="componentLabels" match="gml:node[gml:data/text()='Component']" use="gml:data"/>

<!--DEFAULT RULES-->


<!--MODE: SCHEMATRON-SELECT-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-select-full-path">
<axsl:apply-templates select="." mode="schematron-get-full-path"/>
</axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template match="*" mode="schematron-get-full-path">
<axsl:apply-templates select="parent::*" mode="schematron-get-full-path"/>
<axsl:text>/</axsl:text>
<axsl:choose>
<axsl:when test="namespace-uri()=''">
<axsl:value-of select="name()"/>
<axsl:variable name="p_1" select="1+    count(preceding-sibling::*[name()=name(current())])"/>
<axsl:if test="$p_1&gt;1 or following-sibling::*[name()=name(current())]">[<axsl:value-of select="$p_1"/>]</axsl:if>
</axsl:when>
<axsl:otherwise>
<axsl:text>*[local-name()='</axsl:text>
<axsl:value-of select="local-name()"/>
<axsl:text>' and namespace-uri()='</axsl:text>
<axsl:value-of select="namespace-uri()"/>
<axsl:text>']</axsl:text>
<axsl:variable name="p_2" select="1+   count(preceding-sibling::*[local-name()=local-name(current())])"/>
<axsl:if test="$p_2&gt;1 or following-sibling::*[local-name()=local-name(current())]">[<axsl:value-of select="$p_2"/>]</axsl:if>
</axsl:otherwise>
</axsl:choose>
</axsl:template>
<axsl:template match="@*" mode="schematron-get-full-path">
<axsl:text>/</axsl:text>
<axsl:choose>
<axsl:when test="namespace-uri()=''">@<axsl:value-of select="name()"/>
</axsl:when>
<axsl:otherwise>
<axsl:text>@*[local-name()='</axsl:text>
<axsl:value-of select="local-name()"/>
<axsl:text>' and namespace-uri()='</axsl:text>
<axsl:value-of select="namespace-uri()"/>
<axsl:text>']</axsl:text>
</axsl:otherwise>
</axsl:choose>
</axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-2-->
<!--This mode can be used to generate prefixed XPath for humans-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-2">
<axsl:for-each select="ancestor-or-self::*">
<axsl:text>/</axsl:text>
<axsl:value-of select="name(.)"/>
<axsl:if test="preceding-sibling::*[name(.)=name(current())]">
<axsl:text>[</axsl:text>
<axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/>
<axsl:text>]</axsl:text>
</axsl:if>
</axsl:for-each>
<axsl:if test="not(self::*)">
<axsl:text/>/@<axsl:value-of select="name(.)"/>
</axsl:if>
</axsl:template>

<!--MODE: GENERATE-ID-FROM-PATH -->
<axsl:template match="/" mode="generate-id-from-path"/>
<axsl:template match="text()" mode="generate-id-from-path">
<axsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
<axsl:value-of select="concat('.text-', 1+count(preceding-sibling::text()), '-')"/>
</axsl:template>
<axsl:template match="comment()" mode="generate-id-from-path">
<axsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
<axsl:value-of select="concat('.comment-', 1+count(preceding-sibling::comment()), '-')"/>
</axsl:template>
<axsl:template match="processing-instruction()" mode="generate-id-from-path">
<axsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
<axsl:value-of select="concat('.processing-instruction-', 1+count(preceding-sibling::processing-instruction()), '-')"/>
</axsl:template>
<axsl:template match="@*" mode="generate-id-from-path">
<axsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
<axsl:value-of select="concat('.@', name())"/>
</axsl:template>
<axsl:template match="*" mode="generate-id-from-path" priority="-0.5">
<axsl:apply-templates select="parent::*" mode="generate-id-from-path"/>
<axsl:text>.</axsl:text>
<axsl:value-of select="concat('.',name(),'-',1+count(preceding-sibling::*[name()=name(current())]),'-')"/>
</axsl:template>
<!--MODE: SCHEMATRON-FULL-PATH-3-->
<!--This mode can be used to generate prefixed XPath for humans 
	(Top-level element has index)-->
<axsl:template match="node() | @*" mode="schematron-get-full-path-3">
<axsl:for-each select="ancestor-or-self::*">
<axsl:text>/</axsl:text>
<axsl:value-of select="name(.)"/>
<axsl:if test="parent::*">
<axsl:text>[</axsl:text>
<axsl:value-of select="count(preceding-sibling::*[name(.)=name(current())])+1"/>
<axsl:text>]</axsl:text>
</axsl:if>
</axsl:for-each>
<axsl:if test="not(self::*)">
<axsl:text/>/@<axsl:value-of select="name(.)"/>
</axsl:if>
</axsl:template>

<!--MODE: GENERATE-ID-2 -->
<axsl:template match="/" mode="generate-id-2">U</axsl:template>
<axsl:template match="*" mode="generate-id-2" priority="2">
<axsl:text>U</axsl:text>
<axsl:number level="multiple" count="*"/>
</axsl:template>
<axsl:template match="node()" mode="generate-id-2">
<axsl:text>U.</axsl:text>
<axsl:number level="multiple" count="*"/>
<axsl:text>n</axsl:text>
<axsl:number count="node()"/>
</axsl:template>
<axsl:template match="@*" mode="generate-id-2">
<axsl:text>U.</axsl:text>
<axsl:number level="multiple" count="*"/>
<axsl:text>_</axsl:text>
<axsl:value-of select="string-length(local-name(.))"/>
<axsl:text>_</axsl:text>
<axsl:value-of select="translate(name(),':','.')"/>
</axsl:template>
<!--Strip characters-->
<axsl:template match="text()" priority="-1"/>

<!--SCHEMA METADATA-->
<axsl:template match="/">
<svrl:schematron-output xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" title="ISO schematron file for MEDEA graphml format" schemaVersion="ISO19757-3">
<axsl:comment>
<axsl:value-of select="$archiveDirParameter"/>   
		 <axsl:value-of select="$archiveNameParameter"/>  
		 <axsl:value-of select="$fileNameParameter"/>  
		 <axsl:value-of select="$fileDirParameter"/>
</axsl:comment>
<svrl:ns-prefix-in-attribute-values uri="http://graphml.graphdrawing.org/xmlns" prefix="gml"/>
<svrl:ns-prefix-in-attribute-values uri="http://xml.apache.org/xalan" prefix="xalan"/>
<svrl:active-pattern>
<axsl:attribute name="id">graphml.checks</axsl:attribute>
<axsl:attribute name="name">Checking Model graphml</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M32"/>
<svrl:active-pattern>
<axsl:attribute name="id">File.checks</axsl:attribute>
<axsl:attribute name="name">Checking File nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M33"/>
<svrl:active-pattern>
<axsl:attribute name="id">Component.checks</axsl:attribute>
<axsl:attribute name="name">Checking Component nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M34"/>
<svrl:active-pattern>
<axsl:attribute name="id">InEventPort.checks</axsl:attribute>
<axsl:attribute name="name">Checking InEventPort nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M35"/>
<svrl:active-pattern>
<axsl:attribute name="id">OutEventPort.checks</axsl:attribute>
<axsl:attribute name="name">Checking OutEventPort nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M36"/>
<svrl:active-pattern>
<axsl:attribute name="id">Attribute.checks</axsl:attribute>
<axsl:attribute name="name">Checking Attribute nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M37"/>
<svrl:active-pattern>
<axsl:attribute name="id">Aggregate.checks</axsl:attribute>
<axsl:attribute name="name">Checking Aggregate nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M38"/>
<svrl:active-pattern>
<axsl:attribute name="id">Aggregate.AggregateInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking AggregateInstance nodes contained in Files</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M39"/>
<svrl:active-pattern>
<axsl:attribute name="id">Member.checks</axsl:attribute>
<axsl:attribute name="name">Checking Member nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M40"/>
<svrl:active-pattern>
<axsl:attribute name="id">ComponentAssembly.checks</axsl:attribute>
<axsl:attribute name="name">Checking ComponentAssembly nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M41"/>
<svrl:active-pattern>
<axsl:attribute name="id">ComponentInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking ComponentInstance nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M42"/>
<svrl:active-pattern>
<axsl:attribute name="id">OutEventPortInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking OutEventPortInstance nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M43"/>
<svrl:active-pattern>
<axsl:attribute name="id">InEventPortInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking InEventPortInstance nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M44"/>
<svrl:active-pattern>
<axsl:attribute name="id">AttributeInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking AttributeInstance nodes</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates select="/" mode="M45"/>
</svrl:schematron-output>
</axsl:template>

<!--SCHEMATRON PATTERNS-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">ISO schematron file for MEDEA graphml format</svrl:text>
<axsl:param name="selectID" select="'#ALL'"/>
<axsl:param name="nodeKindKey" select="/gml:graphml/gml:key[@attr.name='kind'][@for='node']/@id"/>
<axsl:param name="nodeLabelKey" select="/gml:graphml/gml:key[@attr.name='label'][@for='node']/@id"/>
<axsl:param name="nodeTypeKey" select="/gml:graphml/gml:key[@attr.name='type'][@for='node']/@id"/>
<axsl:param name="nodeValueKey" select="/gml:graphml/gml:key[@attr.name='value'][@for='node']/@id"/>
<axsl:param name="nodeKeyMemberKey" select="/gml:graphml/gml:key[@attr.name='key'][@for='node']/@id"/>
<axsl:param name="nodeMiddlewareKey" select="/gml:graphml/gml:key[@attr.name='middleware'][@for='node']/@id"/>
<axsl:variable name="PredefinedMemberTypes">
    <Predefined nodeKind="Member" keyName="type" keyType="#ALL" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </axsl:variable>
<axsl:variable name="PredefinedAttributeTypes">
    <Predefined nodeKind="Attribute" keyName="type" keyType="#ALL" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </axsl:variable>
<axsl:variable name="PredefinedAttributeInstanceTypes">
    <Predefined nodeKind="AttributeInstance" keyName="type" keyType="#ALL" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
		<value>Boolean</value>
		<value>Byte</value>		
		<value>Char</value>
		<value>WideChar</value>
		<value>ShortInteger</value>
		<value>LongInteger</value>
		<value>LongLongInteger</value> 
		<value>UnsignedShortInteger</value>
		<value>UnsignedLongInteger</value>
		<value>UnsignedLongLongInteger</value>
		<value>FloatNumber</value>
		<value>DoubleNumber</value> 
		<value>LongDoubleNumber</value>
		<value>String</value>
		<value>WideString</value>
	</Predefined>
  </axsl:variable>
<axsl:variable name="PredefinedBooleanValues">
	<Predefined nodeKind="#ALL" keyName="#ALL" keyType="boolean" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
		<value>true</value>
		<value>false</value>		
	</Predefined>
  </axsl:variable>
<axsl:variable name="PredefinedModelMiddleware">
	<Predefined nodeKind="Model" keyName="middleware" keyType="#ALL" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
		<value>tao</value>
		<value>rtidds</value>		
		<value>opensplice</value>
		<value>tcpip</value>
	</Predefined>	
  </axsl:variable>

<!--PATTERN graphml.checksChecking Model graphml-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking Model graphml</svrl:text>

	<!--RULE -->
<axsl:template match="gml:graphml/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Model']" priority="1000" mode="M32">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:graphml/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Model']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="middleware" select="$node/gml:data[@key=$nodeMiddlewareKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Model Validation Report</svrl:text>
</svrl:successful-report>
</axsl:if>
<axsl:variable name="middlewareList">
			<axsl:for-each select="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', ./)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value[text()=$middleware]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value[text()=$middleware]">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Model must have one of the following middlewares<axsl:text/>
<axsl:value-of select="$middlewareList"/>
<axsl:text/>
</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M32"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M32"/>
<axsl:template match="@*|node()" priority="-2" mode="M32">
<axsl:apply-templates select="@*|*" mode="M32"/>
</axsl:template>

<!--PATTERN File.checksChecking File nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking File nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='File']" priority="1000" mode="M33">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='File']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(key('fileLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'File'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(key('fileLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'File'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File label must be unique within Model</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0         or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0 or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File should contain either Component or Aggregate nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 0        or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0            and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate']) = 0)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 0 or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0 and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate']) = 0)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File with Aggregate nodes can only contain other Aggregate nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) = 0        or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0           and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Component']) = 0)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) = 0 or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0 and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Component']) = 0)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> File with Component nodes can only contain other Component nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M33"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M33"/>
<axsl:template match="@*|node()" priority="-2" mode="M33">
<axsl:apply-templates select="@*|*" mode="M33"/>
</axsl:template>

<!--PATTERN Component.checksChecking Component nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking Component nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Component']" priority="1000" mode="M34">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Component']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="childNode" select="$node/gml:graph/gml:node"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component label must be unique within Model</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='Attribute']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='Attribute']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component should contain InEventPort, OutEventPort and Attribute kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort' and text()!='Attribute']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort' and text()!='Attribute']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component must contain only InEventPort, OutEventPort and Attribute kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M34"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M34"/>
<axsl:template match="@*|node()" priority="-2" mode="M34">
<axsl:apply-templates select="@*|*" mode="M34"/>
</axsl:template>

<!--PATTERN InEventPort.checksChecking InEventPort nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking InEventPort nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPort']" priority="1000" mode="M35">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPort']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort must be the source connection for one link to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="aggregateID" select="//gml:edge[@source=$node/@id]/@target"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort must be connected to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M35"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M35"/>
<axsl:template match="@*|node()" priority="-2" mode="M35">
<axsl:apply-templates select="@*|*" mode="M35"/>
</axsl:template>

<!--PATTERN OutEventPort.checksChecking OutEventPort nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking OutEventPort nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPort']" priority="1000" mode="M36">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPort']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,' \/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,' \/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort must be the source connection for one link to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="aggregateID" select="//gml:edge[@source=$node/@id]/@target"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort must be connected to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M36"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M36"/>
<axsl:template match="@*|node()" priority="-2" mode="M36">
<axsl:apply-templates select="@*|*" mode="M36"/>
</axsl:template>

<!--PATTERN Attribute.checksChecking Attribute nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking Attribute nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Attribute']" priority="1000" mode="M37">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Attribute']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="type" select="$node/gml:data[@key=$nodeTypeKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Attribute Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Attribute label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', ./)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value[text()=$type]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value[text()=$type]">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Attribute must have one of the following types<axsl:text/>
<axsl:value-of select="$typeList"/>
<axsl:text/>
</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M37"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M37"/>
<axsl:template match="@*|node()" priority="-2" mode="M37">
<axsl:apply-templates select="@*|*" mode="M37"/>
</axsl:template>

<!--PATTERN Aggregate.checksChecking Aggregate nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking Aggregate nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Aggregate']" priority="1000" mode="M38">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Aggregate']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="type" select="$node/gml:data[@key=$nodeTypeKey]/text()"/>
<axsl:variable name="childNode" select="$node/gml:graph/gml:node"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']) &gt; 0         or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='AggregateInstance']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']) &gt; 0 or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='AggregateInstance']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate should contain Member and AggregateInstance nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='Member' and text()!='AggregateInstance']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='Member' and text()!='AggregateInstance']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate must contain only Member and AggregateInstance kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M38"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M38"/>
<axsl:template match="@*|node()" priority="-2" mode="M38">
<axsl:apply-templates select="@*|*" mode="M38"/>
</axsl:template>

<!--PATTERN Aggregate.AggregateInstance.checksChecking AggregateInstance nodes contained in Files-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking AggregateInstance nodes contained in Files</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID][parent::gml:graph/parent::gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']]/gml:data[@key=$nodeKindKey][text()='AggregateInstance']" priority="1000" mode="M39">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID][parent::gml:graph/parent::gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']]/gml:data[@key=$nodeKindKey][text()='AggregateInstance']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="parentNode" select="$node/parent::gml:graph/parent::gml:node"/>

		<!--REPORT information-->
<axsl:if test="count($parentNode/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($parentNode/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance must be the source connection for one link to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="aggregateID" select="//gml:edge[@source=$node/@id]/@target"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance must be connected to an Aggregate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M39"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M39"/>
<axsl:template match="@*|node()" priority="-2" mode="M39">
<axsl:apply-templates select="@*|*" mode="M39"/>
</axsl:template>

<!--PATTERN Member.checksChecking Member nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking Member nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Member']" priority="1000" mode="M40">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Member']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="key" select="$node/gml:data[@key=$nodeKeyMemberKey]/text()"/>
<axsl:variable name="type" select="$node/gml:data[@key=$nodeTypeKey]/text()"/>
<axsl:variable name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', ./)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="$key='true' or $key='false'"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$key='true' or $key='false'">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member must have a key of true or false</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value[text()=$type]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value[text()=$type]">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member must have one of the following types<axsl:text/>
<axsl:value-of select="$typeList"/>
<axsl:text/>
</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M40"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M40"/>
<axsl:template match="@*|node()" priority="-2" mode="M40">
<axsl:apply-templates select="@*|*" mode="M40"/>
</axsl:template>

<!--PATTERN ComponentAssembly.checksChecking ComponentAssembly nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking ComponentAssembly nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']" priority="1000" mode="M41">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="childNode" select="$node/gml:graph/gml:node"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly label must be unique within parent graph</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly should contain ComponentAssembly, ComponentInstance, InEventPortDelegate and OutEventPortDelegate kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='ComponentAssembly' and text()!='ComponentInstance' and text()!='InEventPortDelegate' and text()!='OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='ComponentAssembly' and text()!='ComponentInstance' and text()!='InEventPortDelegate' and text()!='OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly must contain only ComponentAssembly, ComponentInstance, InEventPortDelegate and OutEventPortDelegate kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M41"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M41"/>
<axsl:template match="@*|node()" priority="-2" mode="M41">
<axsl:apply-templates select="@*|*" mode="M41"/>
</axsl:template>

<!--PATTERN ComponentInstance.checksChecking ComponentInstance nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking ComponentInstance nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentInstance']" priority="1000" mode="M42">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentInstance']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="childNode" select="$node/gml:graph/gml:node"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance label must be unique within parent ComponentAssembly</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='AttributeInstance']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='AttributeInstance']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance should contain InEventPortInstance, OutEventPortInstance and AttributeInstance kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance' and text()!='AttributeInstance']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance' and text()!='AttributeInstance']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance must contain only InEventPortInstance, OutEventPortInstance and AttributeInstance kind of nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M42"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M42"/>
<axsl:template match="@*|node()" priority="-2" mode="M42">
<axsl:apply-templates select="@*|*" mode="M42"/>
</axsl:template>

<!--PATTERN OutEventPortInstance.checksChecking OutEventPortInstance nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking OutEventPortInstance nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']" priority="1000" mode="M43">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="sourceEventPortID" select="//gml:edge[@target=$node/@id]/@source"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance cannot be a target connection from InEventPortInstance or EventPortDelegate nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="targetEventPortID" select="//gml:edge[@source=$node/@id]/@target"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance should have a source connection to a InEventPortInstance or to an EventPortDelegate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M43"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M43"/>
<axsl:template match="@*|node()" priority="-2" mode="M43">
<axsl:apply-templates select="@*|*" mode="M43"/>
</axsl:template>

<!--PATTERN InEventPortInstance.checksChecking InEventPortInstance nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking InEventPortInstance nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']" priority="1000" mode="M44">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="sourceEventPortID" select="//gml:edge[@target=$node/@id]/@source"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance should have a target connection from a OutEventPortInstance or an EventPortDelegate node</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="targetEventPortID" select="//gml:edge[@source=$node/@id]/@target"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance cannot be a source connection from OutEventPortInstance or EventPortDelegate nodes</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M44"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M44"/>
<axsl:template match="@*|node()" priority="-2" mode="M44">
<axsl:apply-templates select="@*|*" mode="M44"/>
</axsl:template>

<!--PATTERN AttributeInstance.checksChecking AttributeInstance nodes-->
<svrl:text xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl">Checking AttributeInstance nodes</svrl:text>

	<!--RULE -->
<axsl:template match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='AttributeInstance']" priority="1000" mode="M45">
<svrl:fired-rule xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='AttributeInstance']"/>
<axsl:variable name="node" select="parent::gml:node"/>
<axsl:variable name="label" select="$node/gml:data[@key=$nodeLabelKey]/text()"/>
<axsl:variable name="type" select="$node/gml:data[@key=$nodeTypeKey]/text()"/>
<axsl:variable name="value" select="$node/gml:data[@key=$nodeValueKey]/text()"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="typeList">
			<axsl:for-each select="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value" xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
				<axsl:value-of select="concat(', ', ./)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value[text()=$type]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value[text()=$type]">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance must have one of the following types<axsl:text/>
<axsl:value-of select="$typeList"/>
<axsl:text/>
</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$value and $value != ''"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="$value and $value != ''">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance should have an initial value set</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($type = 'Boolean') or ($type = 'Boolean' and ($value = 'true' or $value = 'false'))"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($type = 'Boolean') or ($type = 'Boolean' and ($value = 'true' or $value = 'false'))">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type Boolean must have value of true or false</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber')          or (($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') and (string(number($value)) != 'NaN'))"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') or (($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') and (string(number($value)) != 'NaN'))">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type FloatNumber, or DoubleNumber must have number value</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger')       or (($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.'))  and not(contains($value,'-')))"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') or (($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) and not(contains($value,'-')))">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type Unsigned Integer must have number value without decimal point and without minus sign</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger')       or (($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') or (($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type Integer must have number value without decimal point</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($type = 'Byte' or $type = 'Char' or $type = 'WideChar')       or (($type = 'Byte' or $type = 'Char' or $type = 'WideChar') and string-length($value) = 1)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:svrl="http://purl.oclc.org/dsdl/svrl" test="not($type = 'Byte' or $type = 'Char' or $type = 'WideChar') or (($type = 'Byte' or $type = 'Char' or $type = 'WideChar') and string-length($value) = 1)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type Byte or Char must have only one character in value</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates select="@*|*" mode="M45"/>
</axsl:template>
<axsl:template match="text()" priority="-1" mode="M45"/>
<axsl:template match="@*|node()" priority="-2" mode="M45">
<axsl:apply-templates select="@*|*" mode="M45"/>
</axsl:template>
</axsl:stylesheet>
