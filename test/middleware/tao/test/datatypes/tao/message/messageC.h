// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from
// be/be_codegen.cpp:135

#ifndef _TAO_IDL_MESSAGEC_H_
#define _TAO_IDL_MESSAGEC_H_


#include /**/ "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#include "tao/AnyTypeCode/AnyTypeCode_methods.h"
#include "tao/ORB.h"
#include "tao/SystemException.h"
#include "tao/Basic_Types.h"
#include "tao/ORB_Constants.h"
#include "tao/Object.h"
#include "tao/String_Manager_T.h"
#include "tao/Objref_VarOut_T.h"
#include "tao/VarOut_T.h"
#include /**/ "tao/Versioned_Namespace.h"

#if defined (TAO_EXPORT_MACRO)
#undef TAO_EXPORT_MACRO
#endif
#define TAO_EXPORT_MACRO 

// TAO_IDL - Generated from 
// be/be_visitor_root/root_ch.cpp:62
TAO_BEGIN_VERSIONED_NAMESPACE_DECL



namespace TAO
{
  class Collocation_Proxy_Broker;
  template<typename T> class Narrow_Utils;
}
TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_module/module_ch.cpp:49

namespace Test
{
  
  // TAO_IDL - Generated from
  // be/be_type.cpp:269
  
  struct Message;
  
  typedef
    TAO_Var_Var_T<
        Message
      >
    Message_var;
  
  typedef
    TAO_Out_T<
        Message
      >
    Message_out;
  
  // TAO_IDL - Generated from
  // be/be_visitor_structure/structure_ch.cpp:57
  
  struct  Message
  {
    typedef Message_var _var_type;
    typedef Message_out _out_type;
    
    static void _tao_any_destructor (void *);
    TAO::String_Manager inst_name;
    ::CORBA::Long time;
  };
  
  // TAO_IDL - Generated from
  // be/be_visitor_typecode/typecode_decl.cpp:49
  
  extern  ::CORBA::TypeCode_ptr const _tc_Message;
  
  // TAO_IDL - Generated from
  // be/be_interface.cpp:638

#if !defined (_TEST_HELLO__VAR_OUT_CH_)
#define _TEST_HELLO__VAR_OUT_CH_
  
  class Hello;
  typedef Hello *Hello_ptr;
  
  typedef
    TAO_Objref_Var_T<
        Hello
      >
    Hello_var;
  
  typedef
    TAO_Objref_Out_T<
        Hello
      >
    Hello_out;

#endif /* end #if !defined */
  
  // TAO_IDL - Generated from
  // be/be_visitor_interface/interface_ch.cpp:54

#if !defined (_TEST_HELLO_CH_)
#define _TEST_HELLO_CH_
  
  class  Hello
    : public virtual ::CORBA::Object
  {
  public:
    friend class TAO::Narrow_Utils<Hello>;
    typedef Hello_ptr _ptr_type;
    typedef Hello_var _var_type;
    typedef Hello_out _out_type;
    
    // The static operations.
    static Hello_ptr _duplicate (Hello_ptr obj);
    
    static void _tao_release (Hello_ptr obj);
    
    static Hello_ptr _narrow (::CORBA::Object_ptr obj);
    static Hello_ptr _unchecked_narrow (::CORBA::Object_ptr obj);
    static Hello_ptr _nil (void)
    {
      return static_cast<Hello_ptr> (0);
    }
    
    static void _tao_any_destructor (void *);
    
    // TAO_IDL - Generated from
    // be/be_visitor_operation/operation_ch.cpp:46
    
    virtual void send (
        const ::Test::Message & message);
    
    // TAO_IDL - Generated from
    // be/be_visitor_interface/interface_ch.cpp:216
    
    virtual ::CORBA::Boolean _is_a (const char *type_id);
    virtual const char* _interface_repository_id (void) const;
    virtual ::CORBA::Boolean marshal (TAO_OutputCDR &cdr);
  private:
    TAO::Collocation_Proxy_Broker *the_TAO_Hello_Proxy_Broker_;
  
  protected:
    // Concrete interface only.
    Hello (void);
    
    // These methods travese the inheritance tree and set the
    // parents piece of the given class in the right mode.
    virtual void Test_Hello_setup_collocation (void);
    
    // Concrete non-local interface only.
    Hello (
        IOP::IOR *ior,
        TAO_ORB_Core *orb_core = 0);
    
    // Non-local interface only.
    Hello (
        TAO_Stub *objref,
        ::CORBA::Boolean _tao_collocated = false,
        TAO_Abstract_ServantBase *servant = 0,
        TAO_ORB_Core *orb_core = 0);
    
    virtual ~Hello (void);
  
  private:
    // Private and unimplemented for concrete interfaces.
    Hello (const Hello &);
    
    void operator= (const Hello &);
  };

#endif /* end #if !defined */
  
  // TAO_IDL - Generated from
  // be/be_visitor_typecode/typecode_decl.cpp:49
  
  extern  ::CORBA::TypeCode_ptr const _tc_Hello;

// TAO_IDL - Generated from
// be/be_visitor_module/module_ch.cpp:78

} // module Test

// Proxy Broker Factory function pointer declarations.

// TAO_IDL - Generated from
// be/be_visitor_root/root.cpp:139

extern 
TAO::Collocation_Proxy_Broker *
(*Test__TAO_Hello_Proxy_Broker_Factory_function_pointer) (
    ::CORBA::Object_ptr obj
  );

// TAO_IDL - Generated from
// be/be_visitor_traits.cpp:64

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Traits specializations.
namespace TAO
{

#if !defined (_TEST_HELLO__TRAITS_)
#define _TEST_HELLO__TRAITS_
  
  template<>
  struct  Objref_Traits< ::Test::Hello>
  {
    static ::Test::Hello_ptr duplicate (
        ::Test::Hello_ptr
      );
    static void release (
        ::Test::Hello_ptr
      );
    static ::Test::Hello_ptr nil (void);
    static ::CORBA::Boolean marshal (
        const ::Test::Hello_ptr p,
        TAO_OutputCDR & cdr
      );
  };

#endif /* end #if !defined */
}
TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_structure/any_op_ch.cpp:53


TAO_BEGIN_VERSIONED_NAMESPACE_DECL

 void operator<<= (::CORBA::Any &, const Test::Message &); // copying version
 void operator<<= (::CORBA::Any &, Test::Message*); // noncopying version
 ::CORBA::Boolean operator>>= (const ::CORBA::Any &, Test::Message *&); // deprecated
 ::CORBA::Boolean operator>>= (const ::CORBA::Any &, const Test::Message *&);
TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_interface/any_op_ch.cpp:54



#if defined (ACE_ANY_OPS_USE_NAMESPACE)

namespace Test
{
   void operator<<= ( ::CORBA::Any &, Hello_ptr); // copying
   void operator<<= ( ::CORBA::Any &, Hello_ptr *); // non-copying
   ::CORBA::Boolean operator>>= (const ::CORBA::Any &, Hello_ptr &);
}

#else


TAO_BEGIN_VERSIONED_NAMESPACE_DECL

 void operator<<= (::CORBA::Any &, Test::Hello_ptr); // copying
 void operator<<= (::CORBA::Any &, Test::Hello_ptr *); // non-copying
 ::CORBA::Boolean operator>>= (const ::CORBA::Any &, Test::Hello_ptr &);
TAO_END_VERSIONED_NAMESPACE_DECL



#endif

// TAO_IDL - Generated from
// be/be_visitor_structure/cdr_op_ch.cpp:54


TAO_BEGIN_VERSIONED_NAMESPACE_DECL

 ::CORBA::Boolean operator<< (TAO_OutputCDR &, const Test::Message &);
 ::CORBA::Boolean operator>> (TAO_InputCDR &, Test::Message &);

TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_interface/cdr_op_ch.cpp:55

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

 ::CORBA::Boolean operator<< (TAO_OutputCDR &, const Test::Hello_ptr );
 ::CORBA::Boolean operator>> (TAO_InputCDR &, Test::Hello_ptr &);

TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_codegen.cpp:1219
#if defined (__ACE_INLINE__)
#include "messageC.inl"
#endif /* defined INLINE */

#endif /* ifndef */

