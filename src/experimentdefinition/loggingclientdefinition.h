#ifndef RE_LOGGINGCLIENTDEFINITION_H
#define RE_LOGGINGCLIENTDEFINITION_H

#include "modelentity.h"
#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>

namespace re::Representation {

class LoggingClientDefinition : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::LoggingClientDefinition;
    using LoggingModePbType = network::protocol::experimentdefinition::LoggingMode;

    enum class Mode { Off, Cached, Live };
    explicit LoggingClientDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    Mode mode_;
    double logging_frequency_hz_;
    std::vector<std::string> logged_process_names_;
};

} // namespace re::Representation
#endif // RE_LOGGINGCLIENTDEFINITION_H
