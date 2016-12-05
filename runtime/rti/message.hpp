

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from message.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#ifndef message_874241052_hpp
#define message_874241052_hpp

#include <iosfwd>
#include "messageImpl.h"

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport __declspec(dllexport)
#endif

#include "dds/domain/DomainParticipant.hpp"
#include "dds/topic/TopicTraits.hpp"
#include "dds/core/SafeEnumeration.hpp"
#include "dds/core/String.hpp"
#include "dds/core/array.hpp"
#include "dds/core/vector.hpp"
#include "dds/core/Optional.hpp"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/StructType.hpp"
#include "dds/core/xtypes/UnionType.hpp"
#include "dds/core/xtypes/EnumType.hpp"
#include "dds/core/xtypes/AliasType.hpp"
#include "rti/core/array.hpp"
#include "rti/util/StreamFlagSaver.hpp"
#include "rti/domain/PluginSupport.hpp"
#include "rti/core/LongDouble.hpp"
#include "rti/core/Pointer.hpp"
#include "rti/topic/TopicTraits.hpp"
#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef RTIUSERDllExport
#define RTIUSERDllExport
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

namespace test_dds {

    class NDDSUSERDllExport Message {

      public:
        Message();
        Message(
            int32_t time,
            const dds::core::string& instName,
            const dds::core::string& content);

        #ifdef RTI_CXX11_RVALUE_REFERENCES
        #ifndef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
        Message (Message&& other_) = default;
        Message& operator=(Message&&  other_) = default;
        Message& operator=(const Message&) = default;
        Message(const Message&) = default;
        #else
        Message(Message&& other_) OMG_NOEXCEPT;  
        Message& operator=(Message&&  other_) OMG_NOEXCEPT;
        #endif
        #endif 

        int32_t time() const OMG_NOEXCEPT;
        void time(int32_t value);

        dds::core::string& instName() OMG_NOEXCEPT; 
        const dds::core::string& instName() const OMG_NOEXCEPT;
        void instName(const dds::core::string& value);

        dds::core::string& content() OMG_NOEXCEPT; 
        const dds::core::string& content() const OMG_NOEXCEPT;
        void content(const dds::core::string& value);

        bool operator == (const Message& other_) const;
        bool operator != (const Message& other_) const;

        void swap(Message& other_) OMG_NOEXCEPT ;

      private:

        int32_t m_time_;
        dds::core::string m_instName_;
        dds::core::string m_content_;

    };

    inline void swap(Message& a, Message& b)  OMG_NOEXCEPT 
    {
        a.swap(b);
    }

    NDDSUSERDllExport std::ostream& operator << (std::ostream& o,const Message& sample);

} // namespace test_dds  
namespace dds { 
    namespace topic {

        template<>
        struct topic_type_name<test_dds::Message> {
            NDDSUSERDllExport static std::string value() {
                return "test_dds::Message";
            }
        };

        template<>
        struct is_topic_type<test_dds::Message> : public dds::core::true_type {};

        template<>
        struct topic_type_support<test_dds::Message> {

            NDDSUSERDllExport static void initialize_sample(test_dds::Message& sample);

            NDDSUSERDllExport static void register_type(
                dds::domain::DomainParticipant& participant,
                const std::string & type_name);

            NDDSUSERDllExport static std::vector<char>& to_cdr_buffer(
                std::vector<char>& buffer, const test_dds::Message& sample);

            NDDSUSERDllExport static void from_cdr_buffer(test_dds::Message& sample, const std::vector<char>& buffer);
        };

    }
}

namespace rti { 
    namespace topic {
        template<>
        struct dynamic_type<test_dds::Message> {
            typedef dds::core::xtypes::StructType type;
            NDDSUSERDllExport static const dds::core::xtypes::StructType& get();
        };

        template<>
        struct impl_type<test_dds::Message> {
            typedef test_dds_Message_c type;
        };

    }
}

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif // message_874241052_hpp

