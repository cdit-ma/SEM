#ifndef RE_LOGGINGSERVERDEFINITION_H
#define RE_LOGGINGSERVERDEFINITION_H

#include "modelentity.h"
#include "graphmlparser.h"
namespace re::Representation {

class LoggingClientDefinition;
class LoggingServerDefinition : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::LoggingServerDefinition;
    explicit LoggingServerDefinition(const PbType& pb);
    LoggingServerDefinition(GraphmlParser& graphml_parser, const std::string& medea_id);
    void AddConnectedLoggingClient(const re::Representation::LoggingClientDefinition& logging_client_definition);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    std::string db_file_name_;
    std::vector<sem::types::Uuid> logging_client_definition_uuids_;
};
} // namespace re::Representation

#endif // RE_LOGGINGSERVERDEFINITION_H
