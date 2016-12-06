

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from message.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#include <iosfwd>
#include <iomanip>
#include "message.hpp"
#include "messageImplPlugin.h"

namespace rti_test_dds {

    // ---- Message: 

    Message::Message() :
        m_time_ (0) {
    }   

    Message::Message (
        int32_t time,
        const dds::core::string& instName,
        const dds::core::string& content)
        :
            m_time_( time ),
            m_instName_( instName ),
            m_content_( content ) {
    }

    #ifdef RTI_CXX11_RVALUE_REFERENCES
    #ifdef RTI_CXX11_NO_IMPLICIT_MOVE_OPERATIONS
    Message::Message(Message&& other_) OMG_NOEXCEPT  :m_time_ (std::move(other_.m_time_))
    ,
    m_instName_ (std::move(other_.m_instName_))
    ,
    m_content_ (std::move(other_.m_content_))
    {
    } 

    Message& Message::operator=(Message&&  other_) OMG_NOEXCEPT {
        Message tmp(std::move(other_));
        swap(tmp); 
        return *this;
    }
    #endif
    #endif   

    void Message::swap(Message& other_)  OMG_NOEXCEPT 
    {
        using std::swap;
        swap(m_time_, other_.m_time_);
        swap(m_instName_, other_.m_instName_);
        swap(m_content_, other_.m_content_);
    }  

    bool Message::operator == (const Message& other_) const {
        if (m_time_ != other_.m_time_) {
            return false;
        }
        if (m_instName_ != other_.m_instName_) {
            return false;
        }
        if (m_content_ != other_.m_content_) {
            return false;
        }
        return true;
    }
    bool Message::operator != (const Message& other_) const {
        return !this->operator ==(other_);
    }

    // --- Getters and Setters: -------------------------------------------------
    int32_t rti_test_dds::Message::time() const OMG_NOEXCEPT{
        return m_time_;
    }

    void rti_test_dds::Message::time(int32_t value) {
        m_time_ = value;
    }

    dds::core::string& rti_test_dds::Message::instName() OMG_NOEXCEPT {
        return m_instName_;
    }

    const dds::core::string& rti_test_dds::Message::instName() const OMG_NOEXCEPT {
        return m_instName_;
    }

    void rti_test_dds::Message::instName(const dds::core::string& value) {
        m_instName_ = value;
    }

    dds::core::string& rti_test_dds::Message::content() OMG_NOEXCEPT {
        return m_content_;
    }

    const dds::core::string& rti_test_dds::Message::content() const OMG_NOEXCEPT {
        return m_content_;
    }

    void rti_test_dds::Message::content(const dds::core::string& value) {
        m_content_ = value;
    }

    std::ostream& operator << (std::ostream& o,const Message& sample){
        rti::util::StreamFlagSaver flag_saver (o);
        o <<"[";
        o << "time: " << sample.time()<<", ";
        o << "instName: " << sample.instName()<<", ";
        o << "content: " << sample.content() ;
        o <<"]";
        return o;
    }

} // namespace rti_test_dds  

// --- Type traits: -------------------------------------------------

namespace rti { 
    namespace topic {

        const dds::core::xtypes::StructType& dynamic_type<rti_test_dds::Message>::get()
        {
            return static_cast<const dds::core::xtypes::StructType&>(
                rti::core::native_conversions::cast_from_native<dds::core::xtypes::DynamicType>(
                    *(rti_test_dds_Message_c_get_typecode())));
        }

    }
}  

namespace dds { 
    namespace topic {
        void topic_type_support<rti_test_dds::Message>:: register_type(
            dds::domain::DomainParticipant& participant,
            const std::string& type_name){

            rti::domain::register_type_plugin(
                participant,
                type_name,
                rti_test_dds_Message_cPlugin_new,
                rti_test_dds_Message_cPlugin_delete);
        }

        void topic_type_support<rti_test_dds::Message>::initialize_sample(rti_test_dds::Message& sample){

            rti_test_dds_Message_c* native_sample=reinterpret_cast<rti_test_dds_Message_c*> (&sample);

            struct DDS_TypeDeallocationParams_t deAllocParams = {RTI_FALSE, RTI_FALSE};
            rti_test_dds_Message_c_finalize_w_params(native_sample,&deAllocParams);

            struct DDS_TypeAllocationParams_t allocParams = {RTI_FALSE, RTI_FALSE, RTI_TRUE}; 
            RTIBool ok=rti_test_dds_Message_c_initialize_w_params(native_sample,&allocParams);
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to initialize_w_params");

        } 

        std::vector<char>& topic_type_support<rti_test_dds::Message>::to_cdr_buffer(
            std::vector<char>& buffer, const rti_test_dds::Message& sample)
        {
            // First get the length of the buffer
            unsigned int length = 0;
            RTIBool ok = rti_test_dds_Message_cPlugin_serialize_to_cdr_buffer(
                NULL, &length,reinterpret_cast<const rti_test_dds_Message_c*>(&sample));
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to calculate cdr buffer size");

            // Create a vector with that size and copy the cdr buffer into it
            buffer.resize(length);
            ok = rti_test_dds_Message_cPlugin_serialize_to_cdr_buffer(
                &buffer[0], &length, reinterpret_cast<const rti_test_dds_Message_c*>(&sample));
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to copy cdr buffer");

            return buffer;

        }

        void topic_type_support<rti_test_dds::Message>::from_cdr_buffer(rti_test_dds::Message& sample, 
        const std::vector<char>& buffer)
        {

            RTIBool ok  = rti_test_dds_Message_cPlugin_deserialize_from_cdr_buffer(
                reinterpret_cast<rti_test_dds_Message_c*> (&sample), &buffer[0], 
                static_cast<unsigned int>(buffer.size()));
            rti::core::check_return_code(
                ok ? DDS_RETCODE_OK : DDS_RETCODE_ERROR,
                "Failed to create rti_test_dds::Message from cdr buffer");
        }

    }
}  

