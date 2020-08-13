#include "loggingclientdefinition.h"

namespace re::Representation {
auto ModeFromProto(const LoggingClientDefinition::LoggingModePbType& logging_mode)
    -> LoggingClientDefinition::Mode
{
    switch(logging_mode) {
        case network::protocol::experimentdefinition::Off:
            return LoggingClientDefinition::Mode::Off;
        case network::protocol::experimentdefinition::Live:
            return LoggingClientDefinition::Mode::Live;
        case network::protocol::experimentdefinition::Cached:
            return LoggingClientDefinition::Mode::Cached;
        case network::protocol::experimentdefinition::LoggingMode_INT_MIN_SENTINEL_DO_NOT_USE_:
            [[fallthrough]];
        case network::protocol::experimentdefinition::LoggingMode_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
    }
    throw std::invalid_argument("Invalid logging mode found in LoggingClientDefinition "
                                "modefromproto");
}
auto ModeToProto(const LoggingClientDefinition::Mode& mode)
    -> LoggingClientDefinition::LoggingModePbType
{
    switch(mode) {
        case LoggingClientDefinition::Mode::Cached:
            return LoggingClientDefinition::LoggingModePbType::Cached;
        case LoggingClientDefinition::Mode::Live:
            return LoggingClientDefinition::LoggingModePbType::Live;
        case LoggingClientDefinition::Mode::Off:
            return LoggingClientDefinition::LoggingModePbType::Off;
            break;
    }
    throw std::invalid_argument("Invalid logging mode found in LoggingClientDefinition "
                                "modetoproto");
}
LoggingClientDefinition::LoggingClientDefinition(const LoggingClientDefinition::PbType& pb) :
    DefaultModelEntity(pb.core_data())
{
    mode_ = ModeFromProto(pb.mode());
    logging_frequency_hz_ = pb.frequency();

    for(const auto& logged_process_name : pb.logged_process_names()) {
        logged_process_names_.emplace_back(logged_process_name);
    }
}
auto LoggingClientDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_allocated_core_data(GetCoreData().ToProto().release());
    out->set_mode(ModeToProto(mode_));
    return out;
}
} // namespace re::Representation
