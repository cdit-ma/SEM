#ifndef RE_UUID_H
#define RE_UUID_H

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>
#include <string_view>
namespace sem::types {
class Uuid {
public:
    Uuid();
    explicit Uuid(boost::uuids::uuid b_uuid);
    Uuid(std::string_view string);
    [[nodiscard]] auto to_string() const -> std::string;
    [[nodiscard]] auto hash() const -> std::size_t;

    inline auto operator==(const Uuid& b) const -> bool { return internal_id_ == b.internal_id_; }
    inline auto operator!=(const Uuid& b) const -> bool { return !(*this == b); }

private:
    boost::uuids::uuid internal_id_{};
};

inline auto operator"" _uuid(const char* string, std::size_t size)
{
    std::string_view sv{string, size};
    return Uuid{sv};
}

inline auto operator<<(std::ostream& out, const Uuid& id) -> std::ostream&
{
    return out << id.to_string();
}
} // namespace sem::types

/// Supply std namespaced hash function object s.t. we can use re::uuid as std::unordered map key.
namespace std {
template<> struct hash<sem::types::Uuid> {
    auto operator()(const sem::types::Uuid& id) const -> size_t { return id.hash(); }
};
} // namespace std
#endif // RE_UUID_H
