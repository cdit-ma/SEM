#ifndef RE_SERIALIZABLE_PROTO_HPP
#define RE_SERIALIZABLE_PROTO_HPP

#include <google/protobuf/message.h>
#include <type_traits>
#include "serializable.hpp"
#include <utility>

namespace re::types {
/// Template specialisation of Serializable
///  See src/types/serializable.hpp
template<typename T>
class Serializable<T, std::enable_if_t<std::is_base_of_v<google::protobuf::Message, T>>> {
    T data_;

public:
    explicit Serializable(T in) { data_ = in; }
    explicit Serializable(std::string in) { data_.ParseFromString(in); }
    auto serialize() -> std::string
    {
        std::string out;
        data_.SerializeToString(&out);
        return out;
    }
    static auto serialize(T in) -> std::string
    {
        std::string out;
        in.SerializeToString(&out);
        return out;
    }
    auto deserialize() -> T { return data_; }
    static auto deserialize(std::string in) -> T
    {
        T message;
        message.ParseFromString(in);
        return message;
    }
};
} // namespace re::types

#endif // RE_SERIALIZABLE_PROTO_HPP
