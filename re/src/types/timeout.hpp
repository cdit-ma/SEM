#ifndef RE_TIMEOUT_H
#define RE_TIMEOUT_H

#include <chrono>
#include <variant>

namespace re::types {

class NeverTimeout {
};

using Timeout = std::variant<NeverTimeout, std::chrono::milliseconds>;
} // namespace re::types

#endif // RE_TIMEOUT_H
