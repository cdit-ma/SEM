#ifndef RE_TIMEOUT_H
#define RE_TIMEOUT_H

#include <chrono>
#include <variant>

namespace sem::types {

class NeverTimeout {
};

using Timeout = std::variant<NeverTimeout, std::chrono::milliseconds>;
} // namespace sem::types

#endif // RE_TIMEOUT_H
