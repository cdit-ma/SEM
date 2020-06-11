#include "modelentity.h"
#include "coredata.h"

namespace re::Representation {
DefaultModelEntity::DefaultModelEntity(const DefaultModelEntity::CoreDataPb& pb) : core_data_{pb} {}
} // namespace re::Representation
