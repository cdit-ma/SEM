#include "gtest/gtest.h"
#include <cmrc/cmrc.hpp>
#include <experimentdefinition/experimentdefinition.h>
CMRC_DECLARE(representation_test_models);

namespace re::Representation::test {
TEST(model_representation, basic_test)
{
    auto model_file = cmrc::representation_test_models::get_filesystem().open("models/"
                                                                              "trigger_test."
                                                                              "graphml");
    std::string model_string{model_file.begin(), model_file.end()};

    std::istringstream stream{model_string};

    try {
        Representation::ExperimentDefinition def{stream};
        def.dbg();
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
} // namespace re::Representation::test
