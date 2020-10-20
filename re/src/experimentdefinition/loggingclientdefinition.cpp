#include "loggingclientdefinition.h"
#include "graphmlparser.h"

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
    out->set_frequency(logging_frequency_hz_);

    for(const auto& logged_process_name : logged_process_names_) {
        out->add_logged_process_names(logged_process_name);
    }
    return out;
}
LoggingClientDefinition::LoggingClientDefinition(GraphmlParser& parser,
                                                 const std::string& medea_id) :
    DefaultModelEntity{{sem::types::Uuid{}, medea_id, parser.GetDataValue(medea_id, "label")}}
{
    logging_frequency_hz_ = std::stod(parser.GetDataValue(medea_id, "frequency"));

    // This process name extraction algorithm was copied from the original graphml parser
    //  XXX: Currently untested
    auto processes = parser.GetDataValue(medea_id, "processes_to_log");
    std::transform(processes.begin(), processes.end(), processes.begin(),
                   [](char ch) { return ch == ',' ? ' ' : ch; });

    std::istringstream iss(processes);
    logged_process_names_ = std::vector<std::string>{std::istream_iterator<std::string>{iss},
                                                     std::istream_iterator<std::string>()};

    auto mode_string = parser.GetDataValue(medea_id, "mode");
    if(mode_string == "LIVE") {
        mode_ = Mode::Live;
    } else if(mode_string == "CACHED") {
        mode_ = Mode::Cached;
    } else if(mode_string == "OFF") {
        mode_ = Mode::Off;
    } else {
        throw std::invalid_argument("Found invalid logging profile mode in LoggingClientDefinition");
    }
}
} // namespace re::Representation
