#ifndef RE_COREDATA_H
#define RE_COREDATA_H

#include <network/protocols/experimentdefinition/experimentdefinition.pb.h>
#include "uuid.h"

namespace re::Representation {
class CoreData {
public:
    using PbType = re::network::protocol::experimentdefinition::CoreData;
    explicit CoreData(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

    [[nodiscard]] auto GetUuid() const -> types::Uuid { return uuid_; }
    [[nodiscard]] auto GetMedeaId() const -> std::string { return medea_id_; }
    [[nodiscard]] auto GetMedeaName() const -> std::string { return medea_name_; }

private:
    types::Uuid uuid_;
    std::string medea_id_;
    std::string medea_name_;
};
} // namespace re::Representation

#endif // RE_COREDATA_H
