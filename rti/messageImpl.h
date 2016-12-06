

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from messageImpl.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#ifndef messageImpl_874241103_h
#define messageImpl_874241103_h

#ifndef NDDS_STANDALONE_TYPE
#ifndef ndds_c_h
#include "ndds/ndds_c.h"
#endif
#else
#include "ndds_standalone_type.h"
#endif

extern const char *rti_Message_cTYPENAME;

typedef struct rti_Message_c {

    DDS_Long   time ;
    DDS_Char *   instName ;
    DDS_Char *   content ;

    rti_Message_c() {}
} rti_Message_c ;
#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

NDDSUSERDllExport DDS_TypeCode* rti_Message_c_get_typecode(void); /* Type code */

DDS_SEQUENCE(rti_Message_cSeq, rti_Message_c);                                        

NDDSUSERDllExport
RTIBool rti_Message_c_initialize(
    rti_Message_c* self);

NDDSUSERDllExport
RTIBool rti_Message_c_initialize_ex(
    rti_Message_c* self,RTIBool allocatePointers,RTIBool allocateMemory);

NDDSUSERDllExport
RTIBool rti_Message_c_initialize_w_params(
    rti_Message_c* self,
    const struct DDS_TypeAllocationParams_t * allocParams);        

NDDSUSERDllExport
void rti_Message_c_finalize(
    rti_Message_c* self);

NDDSUSERDllExport
void rti_Message_c_finalize_ex(
    rti_Message_c* self,RTIBool deletePointers);

NDDSUSERDllExport
void rti_Message_c_finalize_w_params(
    rti_Message_c* self,
    const struct DDS_TypeDeallocationParams_t * deallocParams);

NDDSUSERDllExport
void rti_Message_c_finalize_optional_members(
    rti_Message_c* self, RTIBool deletePointers);  

NDDSUSERDllExport
RTIBool rti_Message_c_copy(
    rti_Message_c* dst,
    const rti_Message_c* src);

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif /* messageImpl */

