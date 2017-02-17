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

            <!-- Get all the children Members, VectorInstance, AggregateInstance -->
            <xsl:variable name="members" as="element()*" select="cdit:get_child_entities_of_kind(., 'Member')" />
            <xsl:variable name="vectors" as="element()*" select="cdit:get_child_entities_of_kind(., 'VectorInstance')" />
            <xsl:variable name="aggregates" as="element()*" select="cdit:get_child_entities_of_kind(., 'AggregateInstance')" />
            <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

            <xsl:variable name="aggregate_label" select ="cdit:get_key_value(.,'label')" />
            <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
            <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

            <xsl:variable name="middleware" select="'proto'" />
            <xsl:variable name="mw" select="$middleware" />
            <xsl:variable name="base_mw" select="'base'" />

            <!-- OPEN middleware/proto/label/label.proto -->
            <xsl:result-document href="{concat('middleware/proto/', $aggregate_label_lc, '.proto')}">
                <xsl:message select="concat('middleware/proto/', $aggregate_label_lc, '/', $aggregate_label_lc, '.proto')" />
                <!-- set RE_PATH -->
                <xsl:value-of select="concat('syntax = ', o:dblquote_wrap('proto3'), ';', o:nl())" />
                <xsl:value-of select="concat('package ', $namespace, ';', o:nl())" />
                <xsl:value-of select="o:nl()" />

                 <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:variable name="proto_path" select="concat($datatype, '.proto')" />

                    <xsl:value-of select="concat('import ', o:dblquote_wrap($proto_path), ';', o:nl())" />
                </xsl:for-each-group>

                <xsl:value-of select="concat('message ', $aggregate_label_cc, ' {', o:nl())" />

                <xsl:for-each select="$members">
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
                    <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />
                    <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
                    <xsl:variable name="member_proto_type" select="cdit:get_cpp2proto_type($member_cpp_type)" />
                    
                    <xsl:value-of select="concat(o:t(1), $member_proto_type, ' ', $member_label, ' = ', $sort_order, ';', o:nl())" />
                </xsl:for-each>

                <xsl:for-each select="$vectors">
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type(.)" />
                    <xsl:variable name="vector_proto_type" select="cdit:get_cpp2proto_type($vector_cpp_type)" />
                    
                    <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex(.)" />
                    
                    <xsl:choose>
                        <xsl:when test="$is_vector_complex">
                            <!-- Complex types -->
                            <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                            <xsl:value-of select="concat(o:t(1), 'repeated ', $aggregate_namespace, '.', $vector_cpp_type, ' ', $vector_label, ' = ', $sort_order, ';', o:nl())" />
                        </xsl:when>
                        <xsl:otherwise>
                            <!-- Primitive types -->
                            <xsl:value-of select="concat(o:t(1), 'repeated ', $vector_proto_type, ' ', $vector_label, ' = ', $sort_order, ';', o:nl())" />
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>

                <xsl:for-each select="$aggregates">
                    <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
                    
                    <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
                    <xsl:variable name="aggregate_type" select="cdit:get_key_value(.,'type')" />

                    <!-- Get the Namespace -->
                    <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                    
                    <xsl:value-of select="concat(o:t(1), $aggregate_namespace, '.', $aggregate_type, ' ', $aggregate_label, ' = ', $sort_order, ';', o:nl())" />
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
                <xsl:value-of select="o:cpp_comment('Include the base type')" />
                <!-- path back to root -->
                <xsl:variable name="rel_path" select="'../../../'" />
                <xsl:value-of select="o:local_include(concat($rel_path, 'datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Forward declare the new type -->
                <xsl:value-of select="o:cpp_comment('Forward declare the concrete type')" />
                <xsl:value-of select="o:forward_declare_class($namespace, $aggregate_label_cc)" />

                <!-- Declare convert functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:namespace('proto')" />

                
                <!-- translate functions -->
                <xsl:value-of select="o:tabbed_cpp_comment('Translate Functions', 1)" />
                <xsl:value-of select="concat(o:t(1), $mw_type, '* translate(const ', $base_type ,'* val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $base_type, '* translate(const ', $mw_type ,'* val);', o:nl())" />

                <!-- Helper functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:tabbed_cpp_comment('Helper Functions', 1)" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap('class T'), ' ', $base_type, '* decode(const std::string val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'std::string ', 'encode(const ', $base_type, '* val);', o:nl())" />
                
                <!-- Forward declared template function -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:tabbed_cpp_comment('Forward declare the decode function with concrete type', 1)" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap(''), ' ', $base_type, '* decode', o:angle_wrap($mw_type), '(const std::string val);', o:nl())" />
                
                <xsl:value-of select="concat('};', o:nl())" />

                <!-- End Define Guard -->
                <xsl:value-of select="o:define_guard_end($define_guard)" />

                

            </xsl:result-document>

            <!-- Convert.cpp -->
            <xsl:result-document href="{concat('middleware/', $mw, '/', $aggregate_label_lc, '/convert.cpp')}">
                <!-- Include the header -->
                <xsl:value-of select="o:local_include('convert.h')" />
                <!-- Include the pb generated header -->
                <xsl:value-of select="o:local_include(concat($aggregate_label_lc, '.pb.h'))" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against the base libraries which this message contains -->
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="o:local_include(concat('../', $datatype, '/convert.h'))" />
                </xsl:for-each-group>

                <!-- Define translate functions -->
                <xsl:value-of select="o:cpp_comment('Translate from Base -> Middleware')" />
                <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $base_type, $mw_type, $base_mw, $mw, $middleware)" />

                <xsl:value-of select="o:cpp_comment('Translate from Middleware -> Base')" />
                <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $mw_type, $base_type, $mw, $base_mw, $middleware)" />

                <!-- Define decode functions -->
                <xsl:value-of select="concat('template', o:angle_wrap(''), o:nl())" />
                <xsl:value-of select="concat($base_type, '* ', $mw, '::decode', o:angle_wrap($mw_type), '(const std::string val){', o:nl())" />
                <xsl:value-of select="concat(o:t(1), $mw_type, ' out_;', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'out_.ParseFromString(val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'return translate(', o:and(), 'out_);', o:nl())" />
                <xsl:value-of select="concat('};', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- encode function base->str -->
                <xsl:value-of select="concat('std::string proto::encode(const ', $base_type, '* val){', o:nl())" />
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
                <xsl:value-of select="concat('protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../', $aggregate_label_lc, '.proto)', o:nl())" />
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

                <!-- Include the binary directory which contains the generated pb.h files for other proto libraries which this message contains -->
                <xsl:value-of select="o:cmake_comment('Include the proto binary directory for types required by this type')" />
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="o:cmake_include_dir(concat('${CMAKE_CURRENT_BINARY_DIR}', '/../', $datatype))" />
                </xsl:for-each-group>
                <xsl:value-of select="o:nl()" />

                <!-- Link against Core Libraries -->
                <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against base datatype libraries -->
                <xsl:value-of select="o:cmake_comment('Include the base datatype library which represents this type')" />
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
                <xsl:value-of select="o:nl()" />

                <!-- Link against other proto libraries which this message contains -->
                <xsl:value-of select="o:cmake_comment('Include the proto libraries which are required by this type')" />
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} proto_', $datatype, ')', o:nl())" />
                </xsl:for-each-group>
            </xsl:result-document>




        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>
