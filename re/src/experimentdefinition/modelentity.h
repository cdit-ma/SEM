#ifndef RE_MODELENTITY_H
#define RE_MODELENTITY_H

#include "coredata.h"
#include "experimentdefinition.pb.h"
#include <string>
#include "uuid.h"
namespace re::Representation {

// This *MUST* remain pure virtual to avoid all manner of inheritance, diamond problemey footguns
// and deathtraps.
class ModelEntity {
public:
    [[nodiscard]] virtual auto GetCoreData() const -> CoreData = 0;
};

// Default, convenience implementation of ModelEntity.
class DefaultModelEntity : public ModelEntity {
public:
    using CoreDataPb = re::network::protocol::experimentdefinition::CoreData;
    [[nodiscard]] auto GetCoreData() const -> CoreData final { return core_data_; };
    auto SetCoreData(const CoreData& core_data) -> void { core_data_ = core_data; }
    explicit DefaultModelEntity(const CoreDataPb& pb);
    explicit DefaultModelEntity(CoreData  core_data);

private:
    CoreData core_data_;
};

} // namespace re::Representation
#endif // RE_MODELENTITY_H
