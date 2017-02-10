<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:exsl="http://exslt.org/common"
    xmlns:xalan="http://xml.apache.org/xslt"	
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    exclude-result-prefixes="gml exsl xalan">
		
    
    <xsl:import href="functions.xsl"/>
    <xsl:output method="text" 
        omit-xml-declaration="yes"
        indent="yes" 
		standalone="no"
		xalan:indent-amount="4"/>
	<xsl:strip-space elements="*" />
	
    <xsl:variable name="aggregates" as="element()*" select="cdit:get_entities_of_kind(.,'Aggregate')" />

    <!-- apply all templates starting from the root -->
    <xsl:template match="/">
        <xsl:for-each select="$aggregates">
            <!-- Get the name of the IDL Parent -->
            <xsl:variable name="idl_name" select="cdit:get_key_value(../..,'label')" />

            <!-- Camel Case the Namespace -->
            <xsl:variable name="namespace" select="o:camel_case($idl_name)" />

            <!-- Get the Members -->
            <xsl:variable name="members" as="element()*" select="cdit:get_entities_of_kind(.,'Member')" />
            <xsl:variable name="aggregate_label" select ="cdit:get_key_value(.,'label')" />
            <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
            <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />


            <!-- OPEN middleware/proto/label/label.proto -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '/', $aggregate_label_lc, '.proto')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="concat('syntax = ', o:dblquote_wrap('proto3'), ';', o:nl())" />
                <xsl:value-of select="concat('package ', $namespace, ';', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="concat('message ', $aggregate_label_cc, ' {', o:nl())" />

                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />
                    <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
                    <xsl:variable name="member_proto_type" select="cdit:get_cpp2proto_type($member_cpp_type)" />
                    
                    <xsl:value-of select="concat(o:t(1), $member_proto_type, ' ', $member_label, ' = ', position(), ';', o:nl())" />
                </xsl:for-each>
                
                <xsl:value-of select="concat('}', o:nl())" />
            </xsl:result-document>

            <xsl:variable name="base_type" select="concat('::', $aggregate_label_cc)" />
            <xsl:variable name="mw_type" select="concat($namespace, '::', $aggregate_label_cc)" />

            <!-- TRY CONVERT.H -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '/convert.h')}">
                <xsl:variable name="define_guard" select="upper-case(concat('PROTO_', $aggregate_label, '_CONVERT_H'))" />
                <!-- path back to root -->
                <xsl:variable name="rel_path" select="'../../../'" />

                <!-- Define Guard -->
                <xsl:value-of select="concat('#ifndef ', $define_guard, o:nl())" />
                <xsl:value-of select="concat('#define ', $define_guard, o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include the base message type -->
                <xsl:value-of select="concat('//Include the base type', o:nl())" />
                <xsl:value-of select="concat('#include ', o:dblquote_wrap(concat($rel_path, 'datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h')), o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Forward declare the new type -->
                <xsl:value-of select="concat('//Forward declare the concrete type', o:nl())" />
                <xsl:value-of select="concat('namespace ', $namespace, '{', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'class ', $aggregate_label_cc, ';', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />

                <!-- Declare convert functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="concat('namespace proto{', o:nl())" />
                
                <!-- translate functions -->
                <xsl:value-of select="concat(o:t(1), '//Translate Functions', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $mw_type, '* translate(', $base_type ,'* val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $base_type, '* translate(', $mw_type ,'* val);', o:nl())" />

                <!-- Helper functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="concat(o:t(1), '//Helper Functions', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap('class T'), ' ', $base_type, '* decode(std::string val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'std::string ', 'encode(', $base_type, '* val);', o:nl())" />
                
                <!-- Forward declared template function -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="concat(o:t(1), '//Forward declare the decode function with concrete type', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap(''), ' ', $base_type, '* decode(std::string val);', o:nl())" />
                
                <xsl:value-of select="concat('};', o:nl())" />

                <!-- Define Guard -->
                <xsl:value-of select="concat(o:nl(), '#endif //', $define_guard, o:nl())" />

            </xsl:result-document>

            <!-- TRY CONVERT.CPP -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '/convert.cpp')}">
                <!-- Include the header -->
                <xsl:value-of select="concat('#include ', o:dblquote_wrap(concat($aggregate_label_lc, '.h')), o:nl())" />
                <!-- Include the pb generated file -->
                <xsl:value-of select="concat('#include ', o:dblquote_wrap(concat($aggregate_label_lc, '.pb.h')), o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Translate function base->mw -->
                <xsl:value-of select="concat($mw_type, '* proto::translate(', $base_type ,'* val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'auto out_ = new ', $mw_type, '();', o:nl())" />
                
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:value-of select="concat(o:t(1), 'out_->set_', $member_label, '(val->get_', $member_label ,'());', o:nl())" />
                </xsl:for-each>

                <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />

                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Translate function mw->base -->
                <xsl:value-of select="concat($base_type, '* proto::translate(', $mw_type ,'* val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'auto out_ = new ', $base_type, '();', o:nl())" />
                
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
                    <xsl:value-of select="concat(o:t(1), 'out_->set_', $member_label, '(val->get_', $member_label ,'());', o:nl())" />
                </xsl:for-each>

                <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />

                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- decode function str->mw -->
                <xsl:value-of select="concat('template', o:angle_wrap(''), o:nl())" />
                <xsl:value-of select="concat($base_type, '* proto::decode', o:angle_wrap($mw_type), '(std::string val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $mw_type, ' out_;', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'out_.ParseFromString(val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return translate(', o:and(), 'out_);', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- encode function base->str -->
                <xsl:value-of select="concat('std::string proto::encode(', $base_type, '* val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'std::string out_;', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'auto pb_ = translate(val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'pb', o:fp(), 'SerializeToString(', o:and(), 'out_);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'delete pb_;', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />


                <!--
                proto::Message* proto::translate(::Message* message){
                        auto out = new proto::Message();
                        out->set_time(message->time());
                        out->set_instname(message->instName());
                        out->set_content(message->content());
                        return out;
                }-->
            </xsl:result-document>




        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
