<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<axsl:stylesheet xmlns:axsl="http://www.w3.org/1999/XSL/Transform" xmlns:iso="http://purl.oclc.org/dsdl/schematron" xmlns:sch="http://www.ascc.net/xml/schematron" xmlns:gml="http://graphml.graphdrawing.org/xmlns" gml:dummy-for-xmlns="" xmlns:xalan="http://xml.apache.org/xalan" xalan:dummy-for-xmlns="" version="1.0">
<!--Implementers: please note that overriding process-prolog or process-root is 
    the preferred method for meta-stylesheets to use where possible. -->
<axsl:param name="archiveDirParameter"/>
<axsl:param name="archiveNameParameter"/>
<axsl:param name="fileNameParameter"/>
<axsl:param name="fileDirParameter"/>

<!--PHASES-->


<!--PROLOG-->
<axsl:output xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" indent="yes" standalone="yes" omit-xml-declaration="no" method="xml"/>

<!--KEYS-->
<axsl:key name="fileLabels" match="gml:node[gml:data/text()='IDL']" use="gml:data"/>
<axsl:key name="componentLabels" match="gml:node[gml:data/text()='Component' or gml:data/text()='BlackBox']" use="gml:data"/>

<!--DEFAULT RULES-->


<!--MODE: SCHEMATRON-SELECT-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template mode="schematron-select-full-path" match="*">
<axsl:apply-templates mode="schematron-get-full-path" select="."/>
</axsl:template>

<!--MODE: SCHEMATRON-FULL-PATH-->
<!--This mode can be used to generate an ugly though full XPath for locators-->
<axsl:template mode="schematron-get-full-path" match="*">
<axsl:apply-templates mode="schematron-get-full-path" select="parent::*"/>
<axsl:text>/</axsl:text>
<axsl:choose>
<axsl:when test="namespace-uri()=''">
<axsl:value-of select="name()"/>
<axsl:variable select="1+    count(preceding-sibling::*[name()=name(current())])" name="p_1"/>
<axsl:if test="$p_1&gt;1 or following-sibling::*[name()=name(current())]">[<axsl:value-of select="$p_1"/>]</axsl:if>
</axsl:when>
<axsl:otherwise>
<axsl:text>*[local-name()='</axsl:text>
<axsl:value-of select="local-name()"/>
<axsl:text>' and namespace-uri()='</axsl:text>
<axsl:value-of select="namespace-uri()"/>
<axsl:text>']</axsl:text>
<axsl:variable select="1+   count(preceding-sibling::*[local-name()=local-name(current())])" name="p_2"/>
<axsl:if test="$p_2&gt;1 or following-sibling::*[local-name()=local-name(current())]">[<axsl:value-of select="$p_2"/>]</axsl:if>
</axsl:otherwise>
</axsl:choose>
</axsl:template>
<axsl:template mode="schematron-get-full-path" match="@*">
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
<axsl:template mode="schematron-get-full-path-2" match="node() | @*">
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
<axsl:template mode="generate-id-from-path" match="/"/>
<axsl:template mode="generate-id-from-path" match="text()">
<axsl:apply-templates mode="generate-id-from-path" select="parent::*"/>
<axsl:value-of select="concat('.text-', 1+count(preceding-sibling::text()), '-')"/>
</axsl:template>
<axsl:template mode="generate-id-from-path" match="comment()">
<axsl:apply-templates mode="generate-id-from-path" select="parent::*"/>
<axsl:value-of select="concat('.comment-', 1+count(preceding-sibling::comment()), '-')"/>
</axsl:template>
<axsl:template mode="generate-id-from-path" match="processing-instruction()">
<axsl:apply-templates mode="generate-id-from-path" select="parent::*"/>
<axsl:value-of select="concat('.processing-instruction-', 1+count(preceding-sibling::processing-instruction()), '-')"/>
</axsl:template>
<axsl:template mode="generate-id-from-path" match="@*">
<axsl:apply-templates mode="generate-id-from-path" select="parent::*"/>
<axsl:value-of select="concat('.@', name())"/>
</axsl:template>
<axsl:template priority="-0.5" mode="generate-id-from-path" match="*">
<axsl:apply-templates mode="generate-id-from-path" select="parent::*"/>
<axsl:text>.</axsl:text>
<axsl:value-of select="concat('.',name(),'-',1+count(preceding-sibling::*[name()=name(current())]),'-')"/>
</axsl:template>
<!--MODE: SCHEMATRON-FULL-PATH-3-->
<!--This mode can be used to generate prefixed XPath for humans 
	(Top-level element has index)-->
<axsl:template mode="schematron-get-full-path-3" match="node() | @*">
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
<axsl:template mode="generate-id-2" match="/">U</axsl:template>
<axsl:template priority="2" mode="generate-id-2" match="*">
<axsl:text>U</axsl:text>
<axsl:number count="*" level="multiple"/>
</axsl:template>
<axsl:template mode="generate-id-2" match="node()">
<axsl:text>U.</axsl:text>
<axsl:number count="*" level="multiple"/>
<axsl:text>n</axsl:text>
<axsl:number count="node()"/>
</axsl:template>
<axsl:template mode="generate-id-2" match="@*">
<axsl:text>U.</axsl:text>
<axsl:number count="*" level="multiple"/>
<axsl:text>_</axsl:text>
<axsl:value-of select="string-length(local-name(.))"/>
<axsl:text>_</axsl:text>
<axsl:value-of select="translate(name(),':','.')"/>
</axsl:template>
<!--Strip characters-->
<axsl:template priority="-1" match="text()"/>

<!--SCHEMA METADATA-->
<axsl:template match="/">
<svrl:schematron-output xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" schemaVersion="ISO19757-3" title="ISO schematron file for MEDEA graphml format">
<axsl:comment>
<axsl:value-of select="$archiveDirParameter"/>   
		 <axsl:value-of select="$archiveNameParameter"/>  
		 <axsl:value-of select="$fileNameParameter"/>  
		 <axsl:value-of select="$fileDirParameter"/>
</axsl:comment>
<svrl:ns-prefix-in-attribute-values prefix="gml" uri="http://graphml.graphdrawing.org/xmlns"/>
<svrl:ns-prefix-in-attribute-values prefix="xalan" uri="http://xml.apache.org/xalan"/>
<svrl:active-pattern>
<axsl:attribute name="id">graphml.checks</axsl:attribute>
<axsl:attribute name="name">Checking Model graphml</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M38" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">IDL.checks</axsl:attribute>
<axsl:attribute name="name">Checking IDL entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M39" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">Component.checks</axsl:attribute>
<axsl:attribute name="name">Checking Component entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M40" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">BlackBox.checks</axsl:attribute>
<axsl:attribute name="name">Checking BlackBox entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M41" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">InEventPort.checks</axsl:attribute>
<axsl:attribute name="name">Checking InEventPort entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M42" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">OutEventPort.checks</axsl:attribute>
<axsl:attribute name="name">Checking OutEventPort entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M43" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">Attribute.checks</axsl:attribute>
<axsl:attribute name="name">Checking Attribute entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M44" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">Aggregate.checks</axsl:attribute>
<axsl:attribute name="name">Checking Aggregate entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M45" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">Aggregate.AggregateInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking AggregateInstance entities contained in IDLs</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M46" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">Member.checks</axsl:attribute>
<axsl:attribute name="name">Checking Member entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M47" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">ComponentAssembly.checks</axsl:attribute>
<axsl:attribute name="name">Checking ComponentAssembly entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M48" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">ComponentInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking ComponentInstance entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M49" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">BlackBoxInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking BlackBoxInstance entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M50" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">OutEventPortInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking OutEventPortInstance entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M51" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">InEventPortInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking InEventPortInstance entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M52" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">InEventPortDelegate.checks</axsl:attribute>
<axsl:attribute name="name">Checking InEventPortDelegate entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M53" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">OutEventPortDelegate.checks</axsl:attribute>
<axsl:attribute name="name">Checking OutEventPortDelegate entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M54" select="/"/>
<svrl:active-pattern>
<axsl:attribute name="id">AttributeInstance.checks</axsl:attribute>
<axsl:attribute name="name">Checking AttributeInstance entities</axsl:attribute>
<axsl:apply-templates/>
</svrl:active-pattern>
<axsl:apply-templates mode="M55" select="/"/>
</svrl:schematron-output>
</axsl:template>

<!--SCHEMATRON PATTERNS-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">ISO schematron file for MEDEA graphml format</svrl:text>
<axsl:param select="'#ALL'" name="selectID"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='kind'][@for='node']/@id" name="nodeKindKey"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='label'][@for='node']/@id" name="nodeLabelKey"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='type'][@for='node']/@id" name="nodeTypeKey"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='value'][@for='node']/@id" name="nodeValueKey"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='key'][@for='node']/@id" name="nodeKeyMemberKey"/>
<axsl:param select="/gml:graphml/gml:key[@attr.name='middleware'][@for='node']/@id" name="nodeMiddlewareKey"/>
<axsl:variable name="PredefinedMemberTypes">
    <Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="Member" keyName="type" keyType="#ALL">
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
    <Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="Attribute" keyName="type" keyType="#ALL">
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
    <Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="AttributeInstance" keyName="type" keyType="#ALL">
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
	<Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="#ALL" keyName="#ALL" keyType="boolean">
		<value>true</value>
		<value>false</value>		
	</Predefined>
  </axsl:variable>
<axsl:variable name="PredefinedModelMiddleware">
	<Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="Model" keyName="middleware" keyType="#ALL">
		<value>tao</value>
		<value>rtidds</value>		
		<value>opensplice</value>
		<value>coredx</value>
		<value>tcpip</value>
		<value>qpidpb</value>
	</Predefined>	
  </axsl:variable>
<axsl:variable name="PredefinedProcessActionOnValues">
	<Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="Process" keyName="actionOn" keyType="#ALL">
		<value>Activate</value>
		<value>Configuration_complete</value>
		<value>Preprocess</value>		
		<value>Mainprocess</value>
		<value>Postprocess</value>
		<value>Passivate</value>
		<value>Remove</value>
	</Predefined>	
  </axsl:variable>
<axsl:variable name="PredefinedIDLReservedWords">
	<Predefined xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" nodeKind="#ALL" keyName="label" keyType="#ALL">
		<value>abstract</value>
		<value>any</value>
		<value>attribute</value>		
		<value>boolean</value>
		<value>case</value>
		<value>char</value>
		<value>component</value>
		<value>const</value>
		<value>consumes</value>
		<value>context</value>
		<value>custom</value>
		<value>default</value>
		<value>double</value>
		<value>exception</value>
		<value>emits</value>
		<value>enum</value>
		<value>eventtype</value>
		<value>factory</value>
		<value>false</value>
		<value>finder</value>
		<value>fixed</value>
		<value>float</value>
		<value>getraises</value>
		<value>home</value>
		<value>import</value>
		<value>in</value>
		<value>inout</value>
		<value>interface</value>
		<value>local</value>
		<value>long</value>
		<value>module</value>
		<value>multiple</value>
		<value>native</value>
		<value>object</value>
		<value>octet</value>
		<value>oneway</value>
		<value>out</value>
		<value>primarykey</value>
		<value>private</value>
		<value>provides</value>
		<value>public</value>
		<value>publishes</value>
		<value>raises</value>
		<value>readonly</value>
		<value>setraises</value>
		<value>sequence</value>
		<value>short</value>
		<value>string</value>
		<value>struct</value>
		<value>supports</value>
		<value>switch</value>
		<value>true</value>
		<value>truncatable</value>
		<value>typedef</value>
		<value>typeid</value>
		<value>typeprefix</value>
		<value>unsigned</value>
		<value>union</value>
		<value>uses</value>
		<value>valuebase</value>
		<value>valuetype</value>
		<value>void</value>
		<value>wchar</value>
		<value>wstring</value>
	</Predefined>	
  </axsl:variable>

<!--PATTERN graphml.checksChecking Model graphml-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking Model graphml</svrl:text>

	<!--RULE -->
<axsl:template mode="M38" priority="1000" match="gml:graphml/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Model']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:graphml/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Model']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:data[@key=$nodeMiddlewareKey]/text()" name="middleware"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Model Validation Report</svrl:text>
</svrl:successful-report>
</axsl:if>
<axsl:variable name="middlewareList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value">
				<axsl:value-of select="concat(', ', .)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value[text()=$middleware]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="xalan:nodeset($PredefinedModelMiddleware)/node()[@nodeKind='Model'][@keyName='middleware']//iso:value[text()=$middleware]">
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
<axsl:apply-templates mode="M38" select="@*|*"/>
</axsl:template>
<axsl:template mode="M38" priority="-1" match="text()"/>
<axsl:template mode="M38" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M38" select="@*|*"/>
</axsl:template>

<!--PATTERN IDL.checksChecking IDL entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking IDL entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M39" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='IDL']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='IDL']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(key('fileLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'IDL'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(key('fileLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'IDL'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL label must be unique within Model</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0        or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) &gt; 0        or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0 or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) &gt; 0 or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL should contain either Component, BlackBox or Aggregate entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 0        or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0            and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate']) = 0)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 0 or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0 and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Aggregate']) = 0)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL with Aggregate entities can only contain other Aggregate entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) = 0        or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0           and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Component']) = 0)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) = 0 or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Component']) &gt; 0 and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='Component']) = 0)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL with Component entities can only contain other Component entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) = 0        or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) &gt; 0           and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='BlackBox']) = 0)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) = 0 or (count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='BlackBox']) &gt; 0 and count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()!='BlackBox']) = 0)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> IDL with BlackBox entities can only contain other BlackBox entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M39" select="@*|*"/>
</axsl:template>
<axsl:template mode="M39" priority="-1" match="text()"/>
<axsl:template mode="M39" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M39" select="@*|*"/>
</axsl:template>

<!--PATTERN Component.checksChecking Component entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking Component entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M40" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Component']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Component']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
<axsl:when test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component label must be unique within Model</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='Attribute']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='Attribute']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component should contain InEventPort, OutEventPort and Attribute kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort' and text()!='Attribute']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort' and text()!='Attribute']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Component must contain only InEventPort, OutEventPort and Attribute kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M40" select="@*|*"/>
</axsl:template>
<axsl:template mode="M40" priority="-1" match="text()"/>
<axsl:template mode="M40" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M40" select="@*|*"/>
</axsl:template>

<!--PATTERN BlackBox.checksChecking BlackBox entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking BlackBox entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M41" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBox']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBox']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(key('componentLabels', $label)[gml:data[@key=$nodeKindKey]/text() = 'Component' or gml:data[@key=$nodeKindKey]/text() = 'BlackBox'][gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox label must be unique within Model</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPort']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPort']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox should contain InEventPort and OutEventPort kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPort' and text()!='OutEventPort']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBox must contain only InEventPort and OutEventPort kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M41" select="@*|*"/>
</axsl:template>
<axsl:template mode="M41" priority="-1" match="text()"/>
<axsl:template mode="M41" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M41" select="@*|*"/>
</axsl:template>

<!--PATTERN InEventPort.checksChecking InEventPort entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking InEventPort entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M42" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPort']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPort']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort must be the source connection for one link to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="aggregateID"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPort must be connected to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M42" select="@*|*"/>
</axsl:template>
<axsl:template mode="M42" priority="-1" match="text()"/>
<axsl:template mode="M42" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M42" select="@*|*"/>
</axsl:template>

<!--PATTERN OutEventPort.checksChecking OutEventPort entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking OutEventPort entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M43" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPort']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPort']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,' \/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort must be the source connection for one link to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="aggregateID"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPort must be connected to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M43" select="@*|*"/>
</axsl:template>
<axsl:template mode="M43" priority="-1" match="text()"/>
<axsl:template mode="M43" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M43" select="@*|*"/>
</axsl:template>

<!--PATTERN Attribute.checksChecking Attribute entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking Attribute entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M44" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Attribute']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Attribute']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:data[@key=$nodeTypeKey]/text()" name="type"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Attribute label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Attribute label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="typeList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value">
				<axsl:value-of select="concat(', ', .)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value[text()=$type]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="xalan:nodeset($PredefinedAttributeTypes)/node()[@nodeKind='Attribute'][@keyName='type']//iso:value[text()=$type]">
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
<axsl:apply-templates mode="M44" select="@*|*"/>
</axsl:template>
<axsl:template mode="M44" priority="-1" match="text()"/>
<axsl:template mode="M44" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M44" select="@*|*"/>
</axsl:template>

<!--PATTERN Aggregate.checksChecking Aggregate entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking Aggregate entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M45" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Aggregate']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Aggregate']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:data[@key=$nodeTypeKey]/text()" name="type"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']) &gt; 0         or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='AggregateInstance']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']) &gt; 0 or count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='AggregateInstance']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate should contain Member and AggregateInstance entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']/../gml:data[@key=$nodeKeyMemberKey][text()='true'] ) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/gml:graph/gml:node/gml:data[@key=$nodeKindKey][text()='Member']/../gml:data[@key=$nodeKeyMemberKey][text()='true'] ) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate should contain at least one Member that has a key of true</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='Member' and text()!='AggregateInstance']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='Member' and text()!='AggregateInstance']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Aggregate must contain only Member and AggregateInstance kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M45" select="@*|*"/>
</axsl:template>
<axsl:template mode="M45" priority="-1" match="text()"/>
<axsl:template mode="M45" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M45" select="@*|*"/>
</axsl:template>

<!--PATTERN Aggregate.AggregateInstance.checksChecking AggregateInstance entities contained in IDLs-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking AggregateInstance entities contained in IDLs</svrl:text>

	<!--RULE -->
<axsl:template mode="M46" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID][parent::gml:graph/parent::gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']]/gml:data[@key=$nodeKindKey][text()='AggregateInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID][parent::gml:graph/parent::gml:node/gml:data[@key=$nodeKindKey][text()='Aggregate']]/gml:data[@key=$nodeKindKey][text()='AggregateInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/parent::gml:graph/parent::gml:node" name="parentNode"/>

		<!--REPORT information-->
<axsl:if test="count($parentNode/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($parentNode/gml:data[@key=$nodeKindKey][text()='Aggregate']) &gt; 0">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:edge[@source=$node/@id]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:edge[@source=$node/@id]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance must be the source connection for one link to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="aggregateID"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count(//gml:node[@id=$aggregateID]/gml:data[@key=$nodeKindKey][text()='Aggregate']) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AggregateInstance must be connected to an Aggregate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M46" select="@*|*"/>
</axsl:template>
<axsl:template mode="M46" priority="-1" match="text()"/>
<axsl:template mode="M46" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M46" select="@*|*"/>
</axsl:template>

<!--PATTERN Member.checksChecking Member entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking Member entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M47" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Member']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='Member']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:data[@key=$nodeKeyMemberKey]/text()" name="key"/>
<axsl:variable select="$node/gml:data[@key=$nodeTypeKey]/text()" name="type"/>
<axsl:variable name="typeList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value">
				<axsl:value-of select="concat(', ', .)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable name="reservedList">
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedIDLReservedWords)/node()[@keyName='label']//iso:value">
				<axsl:value-of select="concat(. , ', ')"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not( contains( $reservedList, concat(translate($label,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz'), ', ') ) )">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> Member label must not be an IDL reserved word </svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="$key='true' or $key='false'"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$key='true' or $key='false'">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="xalan:nodeset($PredefinedMemberTypes)/node()[@nodeKind='Member'][@keyName='type']//iso:value[text()=$type]">
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
<axsl:apply-templates mode="M47" select="@*|*"/>
</axsl:template>
<axsl:template mode="M47" priority="-1" match="text()"/>
<axsl:template mode="M47" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M47" select="@*|*"/>
</axsl:template>

<!--PATTERN ComponentAssembly.checksChecking ComponentAssembly entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking ComponentAssembly entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M48" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
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
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentAssembly']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='ComponentInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly should contain ComponentAssembly, ComponentInstance, BlackBoxInstance, InEventPortDelegate and OutEventPortDelegate kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='ComponentAssembly' and text()!='ComponentInstance' and text()!='BlackBoxInstance' and text()!='InEventPortDelegate' and text()!='OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='ComponentAssembly' and text()!='ComponentInstance' and text()!='BlackBoxInstance' and text()!='InEventPortDelegate' and text()!='OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentAssembly must contain only ComponentAssembly, ComponentInstance, BlackBoxInstance, InEventPortDelegate and OutEventPortDelegate kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M48" select="@*|*"/>
</axsl:template>
<axsl:template mode="M48" priority="-1" match="text()"/>
<axsl:template mode="M48" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M48" select="@*|*"/>
</axsl:template>

<!--PATTERN ComponentInstance.checksChecking ComponentInstance entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking ComponentInstance entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M49" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='ComponentInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='AttributeInstance']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance should contain InEventPortInstance, OutEventPortInstance and AttributeInstance kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance' and text()!='AttributeInstance']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance' and text()!='AttributeInstance']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> ComponentInstance must contain only InEventPortInstance, OutEventPortInstance and AttributeInstance kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M49" select="@*|*"/>
</axsl:template>
<axsl:template mode="M49" priority="-1" match="text()"/>
<axsl:template mode="M49" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M49" select="@*|*"/>
</axsl:template>

<!--PATTERN BlackBoxInstance.checksChecking BlackBoxInstance entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking BlackBoxInstance entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M50" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='BlackBoxInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:graph/gml:node" name="childNode"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBoxInstance Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBoxInstance label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBoxInstance label must be unique within parent ComponentAssembly</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0        or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']) &gt; 0 or count($childNode/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBoxInstance should contain InEventPortInstance and OutEventPortInstance kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($childNode/gml:data[@key=$nodeKindKey][text()!='InEventPortInstance' and text()!='OutEventPortInstance']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> BlackBoxInstance must contain only InEventPortInstance and OutEventPortInstance kind of entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M50" select="@*|*"/>
</axsl:template>
<axsl:template mode="M50" priority="-1" match="text()"/>
<axsl:template mode="M50" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M50" select="@*|*"/>
</axsl:template>

<!--PATTERN OutEventPortInstance.checksChecking OutEventPortInstance entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking OutEventPortInstance entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M51" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@target=$node/@id]/@source" name="sourceEventPortID"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance cannot have a target connection from InEventPortInstance or EventPortDelegate entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="targetEventPortID"/>
<axsl:variable select="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." name="targetEventPort"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance should have a source connection to a InEventPortInstance or an EventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortInstance should have the same type as the connected target entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M51" select="@*|*"/>
</axsl:template>
<axsl:template mode="M51" priority="-1" match="text()"/>
<axsl:template mode="M51" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M51" select="@*|*"/>
</axsl:template>

<!--PATTERN InEventPortInstance.checksChecking InEventPortInstance entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking InEventPortInstance entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M52" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@target=$node/@id]/@source" name="sourceEventPortID"/>
<axsl:variable select="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." name="sourceEventPort"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance should have a target connection from an OutEventPortInstance or EventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance should have the same type as the connected source entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="targetEventPortID"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortInstance cannot have a source connection to a OutEventPortInstance or an EventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M52" select="@*|*"/>
</axsl:template>
<axsl:template mode="M52" priority="-1" match="text()"/>
<axsl:template mode="M52" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M52" select="@*|*"/>
</axsl:template>

<!--PATTERN InEventPortDelegate.checksChecking InEventPortDelegate entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking InEventPortDelegate entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M53" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='InEventPortDelegate']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@target=$node/@id]/@source" name="sourceEventPortID"/>
<axsl:variable select="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." name="sourceEventPort"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$sourceEventPortID and count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate should have a target connection from an OutEventPortInstance or EventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate should have the same type as the connected source entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="targetEventPortID"/>
<axsl:variable select="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate']/.." name="targetEventPort"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'OutEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($targetEventPortID) or count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'OutEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate cannot have a source connection to a OutEventPortInstance or an OutEventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> InEventPortDelegate should have the same type as the connected target entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M53" select="@*|*"/>
</axsl:template>
<axsl:template mode="M53" priority="-1" match="text()"/>
<axsl:template mode="M53" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M53" select="@*|*"/>
</axsl:template>

<!--PATTERN OutEventPortDelegate.checksChecking OutEventPortDelegate entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking OutEventPortDelegate entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M54" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='OutEventPortDelegate']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
<axsl:attribute name="role">information</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate Validation Rules</svrl:text>
</svrl:successful-report>
</axsl:if>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate label must not contain \/:*?"&gt;&lt;| or space characters</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate label must be unique within parent ComponentInstance</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@target=$node/@id]/@source" name="sourceEventPortID"/>
<axsl:variable select="//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'OutEventPortInstance' or text() = 'OutEventPortDelegate']/.." name="sourceEventPort"/>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate']) = 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($sourceEventPortID) or count(//gml:node[@id=$sourceEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate']) = 0">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate cannot have a target connection from InEventPortInstance or InEventPortDelegate entities</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$sourceEventPort and (count($sourceEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate should have the same type as the connected source entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:variable select="//gml:edge[@source=$node/@id]/@target" name="targetEventPortID"/>
<axsl:variable select="//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']/.." name="targetEventPort"/>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$targetEventPortID and count(//gml:node[@id=$targetEventPortID]/gml:data[@key=$nodeKindKey][text() = 'InEventPortInstance' or text() = 'InEventPortDelegate' or text() = 'OutEventPortDelegate']) &gt; 0">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate should have a source connection to a InEventPortInstance or an EventPortDelegate entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>

		<!--ASSERT warning-->
<axsl:choose>
<axsl:when test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$targetEventPort and (count($targetEventPort/gml:data[@key=$nodeTypeKey][text() != $node/gml:data[@key=$nodeTypeKey]/text()]) = 0 )">
<axsl:attribute name="role">warning</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> OutEventPortDelegate should have the same type as the connected target entity</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M54" select="@*|*"/>
</axsl:template>
<axsl:template mode="M54" priority="-1" match="text()"/>
<axsl:template mode="M54" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M54" select="@*|*"/>
</axsl:template>

<!--PATTERN AttributeInstance.checksChecking AttributeInstance entities-->
<svrl:text xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema">Checking AttributeInstance entities</svrl:text>

	<!--RULE -->
<axsl:template mode="M55" priority="1000" match="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='AttributeInstance']">
<svrl:fired-rule xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" context="gml:node[$selectID='#ALL' or @id=$selectID]/gml:data[@key=$nodeKindKey][text()='AttributeInstance']"/>
<axsl:variable select="parent::gml:node" name="node"/>
<axsl:variable select="$node/gml:data[@key=$nodeLabelKey]/text()" name="label"/>
<axsl:variable select="$node/gml:data[@key=$nodeTypeKey]/text()" name="type"/>
<axsl:variable select="$node/gml:data[@key=$nodeValueKey]/text()" name="value"/>

		<!--REPORT information-->
<axsl:if test="$node">
<svrl:successful-report xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$node">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="string-length(normalize-space(translate($label,'\/:*?&quot;&gt;&lt;|',''))) = string-length($label)">
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
			<axsl:for-each xmlns="http://purl.oclc.org/dsdl/schematron" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" select="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value">
				<axsl:value-of select="concat(', ', .)"/>
			</axsl:for-each>
		</axsl:variable>

		<!--ASSERT critical-->
<axsl:choose>
<axsl:when test="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value[text()=$type]"/>
<axsl:otherwise>
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="xalan:nodeset($PredefinedAttributeInstanceTypes)/node()[@nodeKind='AttributeInstance'][@keyName='type']//iso:value[text()=$type]">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="count($node/parent::gml:graph/gml:node[gml:data[@key=$nodeLabelKey]/text() = $label]) = 1">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="$value and $value != ''">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($type = 'Boolean') or ($type = 'Boolean' and ($value = 'true' or $value = 'false'))">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') or (($type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber') and (string(number($value)) != 'NaN'))">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') or (($type = 'UnsignedShortInteger' or $type = 'UnsignedLongInteger' or $type = 'UnsignedLongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) and not(contains($value,'-')))">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') or (($type = 'ShortInteger' or $type = 'LongInteger' or $type = 'LongLongInteger') and string(number($value)) != 'NaN' and not(contains($value,'.')) )">
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
<svrl:failed-assert xmlns:svrl="http://purl.oclc.org/dsdl/svrl" xmlns:schold="http://www.ascc.net/xml/schematron" xmlns:xs="http://www.w3.org/2001/XMLSchema" test="not($type = 'Byte' or $type = 'Char' or $type = 'WideChar') or (($type = 'Byte' or $type = 'Char' or $type = 'WideChar') and string-length($value) = 1)">
<axsl:attribute name="role">critical</axsl:attribute>
<svrl:text>[<axsl:text/>
<axsl:value-of select="$node/@id"/>
<axsl:text/>] <axsl:text/>
<axsl:value-of select="$label"/>
<axsl:text/> AttributeInstance of type Byte or Char must have only one character in value</svrl:text>
</svrl:failed-assert>
</axsl:otherwise>
</axsl:choose>
<axsl:apply-templates mode="M55" select="@*|*"/>
</axsl:template>
<axsl:template mode="M55" priority="-1" match="text()"/>
<axsl:template mode="M55" priority="-2" match="@*|node()">
<axsl:apply-templates mode="M55" select="@*|*"/>
</axsl:template>
</axsl:stylesheet>
