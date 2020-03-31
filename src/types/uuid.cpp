#include "uuid.h"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace re::types {

Uuid::Uuid(boost::uuids::uuid b_uuid) : internal_id_(b_uuid) {}

Uuid::Uuid(std::string_view string)
{
    boost::uuids::string_generator string_generator;
    internal_id_ = string_generator(string.data());
}

Uuid::Uuid()
{
    boost::uuids::random_generator random_generator;
    internal_id_ = random_generator();
}

auto Uuid::to_string() const -> std::string
{
    return boost::uuids::to_string(internal_id_);
}

auto Uuid::hash() const -> std::size_t
{
    return boost::hash<boost::uuids::uuid>()(internal_id_);
}
} // namespace re::types
