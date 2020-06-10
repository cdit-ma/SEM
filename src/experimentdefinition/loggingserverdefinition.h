#ifndef RE_LOGGINGSERVERDEFINITION_H
#define RE_LOGGINGSERVERDEFINITION_H

#include "modelentity.h"
namespace re::Representation {

class LoggingServerDefinition : public DefaultModelEntity{
public:
    using PbType = network::protocol::experimentdefinition::LoggingServerDefinition;
    explicit LoggingServerDefinition(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;
private:
    std::string db_file_name_;
    std::vector<types::Uuid> logging_client_definition_uuids_;
};
} // namespace re::Representation

#endif // RE_LOGGINGSERVERDEFINITION_H
