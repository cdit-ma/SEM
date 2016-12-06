

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from messageImpl.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#ifndef NDDS_STANDALONE_TYPE
#ifndef ndds_c_h
#include "ndds/ndds_c.h"
#endif

#ifndef cdr_type_h
#include "cdr/cdr_type.h"
#endif    

#ifndef osapi_heap_h
#include "osapi/osapi_heap.h" 
#endif
#else
#include "ndds_standalone_type.h"
#endif

#include "messageImpl.h"

/* ========================================================================= */
const char *rti_Message_cTYPENAME = "rti::Message";

DDS_TypeCode* rti_Message_c_get_typecode()
{
    static RTIBool is_initialized = RTI_FALSE;

    static DDS_TypeCode rti_Message_c_g_tc_instName_string = DDS_INITIALIZE_STRING_TYPECODE((255));
    static DDS_TypeCode rti_Message_c_g_tc_content_string = DDS_INITIALIZE_STRING_TYPECODE((255));
    static DDS_TypeCode_Member rti_Message_c_g_tc_members[3]=
    {

        {
            (char *)"time",/* Member name */
            {
                0,/* Representation ID */          
                DDS_BOOLEAN_FALSE,/* Is a pointer? */
                -1, /* Bitfield bits */
                NULL/* Member type code is assigned later */
            },
            0, /* Ignored */
            0, /* Ignored */
            0, /* Ignored */
            NULL, /* Ignored */
            RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
            DDS_PUBLIC_MEMBER,/* Member visibility */
            1,
            NULL/* Ignored */
        }, 
        {
            (char *)"instName",/* Member name */
            {
                1,/* Representation ID */          
                DDS_BOOLEAN_FALSE,/* Is a pointer? */
                -1, /* Bitfield bits */
                NULL/* Member type code is assigned later */
            },
            0, /* Ignored */
            0, /* Ignored */
            0, /* Ignored */
            NULL, /* Ignored */
            RTI_CDR_KEY_MEMBER , /* Is a key? */
            DDS_PUBLIC_MEMBER,/* Member visibility */
            1,
            NULL/* Ignored */
        }, 
        {
            (char *)"content",/* Member name */
            {
                2,/* Representation ID */          
                DDS_BOOLEAN_FALSE,/* Is a pointer? */
                -1, /* Bitfield bits */
                NULL/* Member type code is assigned later */
            },
            0, /* Ignored */
            0, /* Ignored */
            0, /* Ignored */
            NULL, /* Ignored */
            RTI_CDR_REQUIRED_MEMBER, /* Is a key? */
            DDS_PUBLIC_MEMBER,/* Member visibility */
            1,
            NULL/* Ignored */
        }
    };

    static DDS_TypeCode rti_Message_c_g_tc =
    {{
            DDS_TK_STRUCT,/* Kind */
            DDS_BOOLEAN_FALSE, /* Ignored */
            -1, /*Ignored*/
            (char *)"rti::Message", /* Name */
            NULL, /* Ignored */      
            0, /* Ignored */
            0, /* Ignored */
            NULL, /* Ignored */
            3, /* Number of members */
            rti_Message_c_g_tc_members, /* Members */
            DDS_VM_NONE  /* Ignored */         
        }}; /* Type code for rti_Message_c*/

    if (is_initialized) {
        return &rti_Message_c_g_tc;
    }

    rti_Message_c_g_tc_members[0]._representation._typeCode = (RTICdrTypeCode *)&DDS_g_tc_long;

    rti_Message_c_g_tc_members[1]._representation._typeCode = (RTICdrTypeCode *)&rti_Message_c_g_tc_instName_string;

    rti_Message_c_g_tc_members[2]._representation._typeCode = (RTICdrTypeCode *)&rti_Message_c_g_tc_content_string;

    is_initialized = RTI_TRUE;

    return &rti_Message_c_g_tc;
}

RTIBool rti_Message_c_initialize(
    rti_Message_c* sample) {
    return rti_Message_c_initialize_ex(sample,RTI_TRUE,RTI_TRUE);
}

RTIBool rti_Message_c_initialize_ex(
    rti_Message_c* sample,RTIBool allocatePointers, RTIBool allocateMemory)
{

    struct DDS_TypeAllocationParams_t allocParams =
    DDS_TYPE_ALLOCATION_PARAMS_DEFAULT;

    allocParams.allocate_pointers =  (DDS_Boolean)allocatePointers;
    allocParams.allocate_memory = (DDS_Boolean)allocateMemory;

    return rti_Message_c_initialize_w_params(
        sample,&allocParams);

}

RTIBool rti_Message_c_initialize_w_params(
    rti_Message_c* sample, const struct DDS_TypeAllocationParams_t * allocParams)
{

    if (allocParams) {} /* To avoid warnings */

    if (!RTICdrType_initLong(&sample->time)) {
        return RTI_FALSE;
    }     

    if (allocParams->allocate_memory){
        sample->instName= DDS_String_alloc ((255));
        if (sample->instName == NULL) {
            return RTI_FALSE;
        }

    } else {
        if (sample->instName!= NULL) { 
            sample->instName[0] = '\0';
        }
    }

    if (allocParams->allocate_memory){
        sample->content= DDS_String_alloc ((255));
        if (sample->content == NULL) {
            return RTI_FALSE;
        }

    } else {
        if (sample->content!= NULL) { 
            sample->content[0] = '\0';
        }
    }

    return RTI_TRUE;
}

void rti_Message_c_finalize(
    rti_Message_c* sample)
{

    rti_Message_c_finalize_ex(sample,RTI_TRUE);
}

void rti_Message_c_finalize_ex(
    rti_Message_c* sample,RTIBool deletePointers)
{
    struct DDS_TypeDeallocationParams_t deallocParams =
    DDS_TYPE_DEALLOCATION_PARAMS_DEFAULT;

    if (sample==NULL) {
        return;
    } 

    deallocParams.delete_pointers = (DDS_Boolean)deletePointers;

    rti_Message_c_finalize_w_params(
        sample,&deallocParams);
}

void rti_Message_c_finalize_w_params(
    rti_Message_c* sample,const struct DDS_TypeDeallocationParams_t * deallocParams)
{

    if (sample==NULL) {
        return;
    }
    if (deallocParams) {} /* To avoid warnings */

    if (sample->instName != NULL) {
        DDS_String_free(sample->instName);
        sample->instName=NULL;

    }
    if (sample->content != NULL) {
        DDS_String_free(sample->content);
        sample->content=NULL;

    }
}

void rti_Message_c_finalize_optional_members(
    rti_Message_c* sample, RTIBool deletePointers)
{
    struct DDS_TypeDeallocationParams_t deallocParamsTmp =
    DDS_TYPE_DEALLOCATION_PARAMS_DEFAULT;
    struct DDS_TypeDeallocationParams_t * deallocParams =
    &deallocParamsTmp;

    if (sample==NULL) {
        return;
    } 
    if (deallocParams) {} /* To avoid warnings */

    deallocParamsTmp.delete_pointers = (DDS_Boolean)deletePointers;
    deallocParamsTmp.delete_optional_members = DDS_BOOLEAN_TRUE;

}

RTIBool rti_Message_c_copy(
    rti_Message_c* dst,
    const rti_Message_c* src)
{

    if (!RTICdrType_copyLong (
        &dst->time, &src->time)) { 
        return RTI_FALSE;
    }
    if (!RTICdrType_copyStringEx (
        &dst->instName, src->instName, 
        (255) + 1,RTI_TRUE)){
        return RTI_FALSE;
    }
    if (!RTICdrType_copyStringEx (
        &dst->content, src->content, 
        (255) + 1,RTI_TRUE)){
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

/**
* <<IMPLEMENTATION>>
*
* Defines:  TSeq, T
*
* Configure and implement 'rti_Message_c' sequence class.
*/
#define T rti_Message_c
#define TSeq rti_Message_cSeq
#define T_initialize_w_params rti_Message_c_initialize_w_params
#define T_finalize_w_params   rti_Message_c_finalize_w_params
#define T_copy       rti_Message_c_copy

#ifndef NDDS_STANDALONE_TYPE
#include "dds_c/generic/dds_c_sequence_TSeq.gen"
#else
#include "dds_c_sequence_TSeq.gen"
#endif

#undef T_copy
#undef T_finalize_w_params
#undef T_initialize_w_params
#undef TSeq
#undef T

