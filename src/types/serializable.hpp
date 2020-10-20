#ifndef RE_SERIALIZABLE_HPP
#define RE_SERIALIZABLE_HPP

#include <string>
#include <type_traits>
#include <utility>

namespace sem::types {

/// This is a template interface showing the methods any template specialization should implement.
/// This template interface should be implemented for any class that needs to be
///  serialized/deserialized for "over the wire transport".
/// See src/types/proto/serializable.hpp for an example specialization.
template<typename T, class Enable = void> class Serializable {
public:
    explicit Serializable(T in) = delete;
    explicit Serializable(const std::string& serialized) = delete;
    auto serialize() -> std::string = delete;
    static auto deserialize(const std::string&) -> T = delete;
    auto deserialize() -> T = delete;
};
} // namespace sem::types

#endif // RE_SERIALIZABLE_HPP
