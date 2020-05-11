#include "modelentity.h"

#include <utility>
#include "coredata.h"

namespace re::Representation {
DefaultModelEntity::DefaultModelEntity(const DefaultModelEntity::CoreDataPb& pb) : core_data_{pb} {}
DefaultModelEntity::DefaultModelEntity(CoreData core_data) : core_data_{std::move(core_data)} {}
} // namespace re::Representation
