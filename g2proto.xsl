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

                <!-- Define Guard -->
                <xsl:value-of select="o:define_guard($define_guard)" />

                <!-- Include the base message type -->
                <xsl:value-of select="concat('//Include the base type', o:nl())" />
                <!-- path back to root -->
                <xsl:variable name="rel_path" select="'../../../'" />
                <xsl:value-of select="o:local_include(concat($rel_path, 'datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Forward declare the new type -->
                <xsl:value-of select="concat('//Forward declare the concrete type', o:nl())" />
                <xsl:value-of select="o:forward_declare_class($namespace, $aggregate_label_cc)" />

                <!-- Declare convert functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:namespace('proto')" />

                
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
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap(''), ' ', $base_type, '* decode', o:angle_wrap($mw_type), '(std::string val);', o:nl())" />
                
                <xsl:value-of select="concat('};', o:nl())" />

                <!-- End Define Guard -->
                <xsl:value-of select="o:define_guard_end($define_guard)" />

                

            </xsl:result-document>

            <!-- Convert.cpp -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '/convert.cpp')}">
                <!-- Include the header -->
                <xsl:value-of select="o:local_include('convert.h')" />
                <!-- Include the pb generated header -->
                <xsl:value-of select="o:local_include(concat($aggregate_label_lc, '.pb.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Translate function base->mw -->
                <xsl:value-of select="concat($mw_type, '* proto::translate(', $base_type ,'* val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'auto out_ = new ', $mw_type, '();', o:nl())" />
                
                 <!-- mw->set(get->get()) -->
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />

                    <xsl:value-of select="concat(o:t(1), 'out_', o:fp(), o:cpp_proto_set_func($member_label) , '(val', o:fp(), o:cpp_base_get_func($member_label) ,'());', o:nl())" />
                </xsl:for-each>

                <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />

                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Translate function mw->base -->
                <xsl:value-of select="concat($base_type, '* proto::translate(', $mw_type ,'* val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'auto out_ = new ', $base_type, '();', o:nl())" />
                
                <!-- base->set(mw->get()) -->
                <xsl:for-each select="$members">
                    <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />

                    <xsl:value-of select="concat(o:t(1), 'out_', o:fp(), o:cpp_base_set_func($member_label) , '(val', o:fp(), o:cpp_proto_get_func($member_label) ,'());', o:nl())" />
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
                <xsl:value-of select="concat(o:t(1), 'pb_', o:fp(), 'SerializeToString(', o:and(), 'out_);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'delete pb_;', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />
            </xsl:result-document>

            <!-- Output CMakeLists.txt -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '/CMakeLists.txt')}">
                <!-- set RE_PATH -->
                <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
                <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
                <xsl:value-of select="o:cmake_set_env('CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}', '${RE_PATH}/cmake/modules')" />
                <xsl:value-of select="o:nl()" />

                <!-- Set PROJ_NAME -->
                <xsl:variable name="proj_name" select="concat('proto_', $aggregate_label_lc)" />
                <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

                <!-- Find re_core -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
                <xsl:value-of select="o:nl()" />

                <!-- Find protobuf -->
                <xsl:value-of select="o:cmake_find_package('Protobuf')" />
                <xsl:value-of select="o:nl()" />
                
                <!-- Generate PB files -->
                <xsl:value-of select="concat('protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ', $aggregate_label_lc, '.proto)', o:nl())" />
                <xsl:value-of select="o:nl()" />
                
                <!-- Set Source files -->
                <xsl:value-of select="concat('set(SOURCE', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${PROTO_SRCS}', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Set Headers files -->
                <xsl:value-of select="concat('set(HEADERS', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${PROTO_HDRS}', o:nl())" />
                <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.h', o:nl())" />
                <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Include directories -->
                <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
                <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
                <xsl:value-of select="o:nl()" />
                
                <xsl:value-of select="o:cmake_comment('Include the Protobuf directory')" />
                <xsl:value-of select="o:cmake_include_dir('${PROTOBUF_INCLUDE_DIRS}')" />
                <xsl:value-of select="o:nl()" />

                <xsl:value-of select="o:cmake_comment('Include the current binary directory to enable linking of pb.h')" />
                <xsl:value-of select="o:cmake_include_dir('${CMAKE_CURRENT_BINARY_DIR}')" />
                <xsl:value-of select="o:nl()" />

                <!-- add library/link -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
            </xsl:result-document>




        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
