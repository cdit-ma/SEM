#ifndef RE_WORKERINSTANCE_H
#define RE_WORKERINSTANCE_H

#include "modelentity.h"
#include <types/uuid.h>
namespace re::Representation {

class WorkerInstance : public DefaultModelEntity {
public:
    using PbType = network::protocol::experimentdefinition::WorkerInstance;
    explicit WorkerInstance(const PbType& pb);
    [[nodiscard]] auto ToProto() const -> std::unique_ptr<PbType>;

private:
    types::Uuid definition_uuid_;

    std::vector<types::Uuid> attribute_instance_uuids_;
};
} // namespace re::Representation

#endif // RE_WORKERINSTANCE_H
