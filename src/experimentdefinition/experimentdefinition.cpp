#include "experimentdefinition.h"
namespace re::Representation {
auto ExperimentDefinition::ToProto() const -> std::unique_ptr<PbType>
{
    auto out = std::make_unique<PbType>();
    out->set_uuid(uuid_.to_string());
    out->set_experiment_name(experiment_name_);

    auto& attribute_map = *out->mutable_attribute_definitions();
    for(const auto& [uuid, attribute_definition] : attribute_definitions_) {
        attribute_map[uuid.to_string()] = *attribute_definition.ToProto();
    }

    auto& attr_instance_map = *out->mutable_attribute_instances();
    for(const auto& [uuid, attribute_instance] : attribute_instances_) {
        attr_instance_map[uuid.to_string()] = *attribute_instance->ToProto();
    }

    auto& component_map = *out->mutable_component_definitions();
    for(const auto& [uuid, component] : component_definitions_) {
        component_map[uuid.to_string()] = *component.ToProto();
    }

    auto& component_instance_map = *out->mutable_component_instances();
    for(const auto& [uuid, component_instance] : component_instances_) {
        component_instance_map[uuid.to_string()] = *component_instance.ToProto();
    }

    auto& assembly_map = *out->mutable_component_assemblies();
    for(const auto& [uuid, assembly] : component_assemblies_) {
        assembly_map[uuid.to_string()] = *assembly.ToProto();
    }

    auto& delegate_map = *out->mutable_port_delegates();
    for(const auto& [uuid, delegate] : port_delegates_) {
        delegate_map[uuid.to_string()] = *delegate.ToProto();
    }

    auto& container_map = *out->mutable_containers();
    for(const auto& [uuid, container] : container_definitions_) {
        container_map[uuid.to_string()] = *container.ToProto();
    }

    auto& mw_port_def_map = *out->mutable_middleware_port_definitions();
    for(const auto& [uuid, mw_port_def] : middleware_port_definitions_) {
        mw_port_def_map[uuid.to_string()] = *mw_port_def.ToProto();
    }

    auto& mw_port_inst_map = *out->mutable_middleware_port_instances();
    for(const auto& [uuid, mw_port_inst] : middleware_port_instances_) {
        mw_port_inst_map[uuid.to_string()] = *mw_port_inst.ToProto();
    }

    auto& periodic_port_def_map = *out->mutable_periodic_port_definitions();
    for(const auto& [uuid, periodic_port_def] : periodic_port_definitions_) {
        periodic_port_def_map[uuid.to_string()] = *periodic_port_def.ToProto();
    }

    auto& periodic_port_inst_map = *out->mutable_periodic_port_instances();
    for(const auto& [uuid, port_inst] : periodic_port_instances_) {
        periodic_port_inst_map[uuid.to_string()] = *port_inst.ToProto();
    }

    auto& worker_definitions_map = *out->mutable_worker_definitions();
    for(const auto& [uuid, worker_definition] : worker_definitions_) {
        worker_definitions_map[uuid.to_string()] = *worker_definition.ToProto();
    }

    auto& worker_instances_map = *out->mutable_worker_instances();
    for(const auto& [uuid, worker_instance] : worker_instances_) {
        worker_instances_map[uuid.to_string()] = *worker_instance.ToProto();
    }

    auto& node_defs_map = *out->mutable_nodes();
    for(const auto& [uuid, node] : node_definitions_) {
        node_defs_map[uuid.to_string()] = *node.ToProto();
    }
    auto& cluster_defs_map = *out->mutable_clusters();
    for(const auto& [uuid, cluster] : cluster_definitions_) {
        cluster_defs_map[uuid.to_string()] = *cluster.ToProto();
    }

    auto& trigger_defs_map = *out->mutable_trigger_definitions();
    for(const auto& [uuid, trigger] : trigger_definitions_) {
        trigger_defs_map[uuid.to_string()] = *trigger.ToProto();
    }

    auto& trigger_inst_map = *out->mutable_trigger_instances();
    for(const auto& [uuid, trigger] : trigger_instances_) {
        trigger_inst_map[uuid.to_string()] = *trigger.ToProto();
    }
    auto& strategy_map = *out->mutable_strategies();
    for(const auto& [uuid, strategy] : strategies_) {
        strategy_map[uuid.to_string()] = *strategy.ToProto();
    }

    auto& logging_client_map = *out->mutable_logging_client_definitions();
    for(const auto& [uuid, logging_client] : logging_client_definitions_) {
        logging_client_map[uuid.to_string()] = *logging_client.ToProto();
    }

    auto& logging_server_map = *out->mutable_logging_server_definitions();
    for(const auto& [uuid, logging_server] : logging_server_definitions_) {
        logging_server_map[uuid.to_string()] = *logging_server.ToProto();
    }

    return out;
}
ExperimentDefinition::ExperimentDefinition(const ExperimentDefinition::PbType& definition_pb)
{
    experiment_name_ = definition_pb.experiment_name();
    uuid_ = types::Uuid{definition_pb.uuid()};

    for(const auto& [uuid, attribute_definition] : definition_pb.attribute_definitions()) {
        attribute_definitions_.insert(
            {types::Uuid(uuid), AttributeDefinition{attribute_definition}});
    }

    for(const auto& [uuid, attribute_instance] : definition_pb.attribute_instances()) {
        attribute_instances_.insert(
            {types::Uuid{uuid}, ConstructAttributeInstance(attribute_instance)});
    }

    for(const auto& [uuid, component_definition] : definition_pb.component_definitions()) {
        component_definitions_.insert(
            {types::Uuid{uuid}, ComponentDefinition{component_definition}});
    }

    for(const auto& [uuid, component_instance] : definition_pb.component_instances()) {
        component_instances_.insert({types::Uuid{uuid}, ComponentInstance{component_instance}});
    }

    for(const auto& [uuid, component_assembly] : definition_pb.component_assemblies()) {
        component_assemblies_.insert({types::Uuid{uuid}, ComponentAssembly{component_assembly}});
    }

    for(const auto& [uuid, container] : definition_pb.containers()) {
        container_definitions_.insert({types::Uuid{uuid}, Container{container}});
    }

    for(const auto& [uuid, middleware_port_definition] :
        definition_pb.middleware_port_definitions()) {
        middleware_port_definitions_.insert(
            {types::Uuid{uuid}, MiddlewarePortDefinition{middleware_port_definition}});
    }

    for(const auto& [uuid, middleware_port_instance] : definition_pb.middleware_port_instances()) {
        middleware_port_instances_.insert(
            {types::Uuid{uuid}, MiddlewarePortInstance{middleware_port_instance}});
    }

    for(const auto& [uuid, periodic_port_definition] : definition_pb.periodic_port_definitions()) {
        periodic_port_definitions_.insert(
            {types::Uuid{uuid}, PeriodicPortDefinition{periodic_port_definition}});
    }

    for(const auto& [uuid, periodic_port_instance] : definition_pb.periodic_port_instances()) {
        periodic_port_instances_.insert(
            {types::Uuid{uuid}, PeriodicPortInstance{periodic_port_instance}});
    }

    for(const auto& [uuid, port_delegate] : definition_pb.port_delegates()) {
        port_delegates_.insert({types::Uuid{uuid}, PortDelegateInstance{port_delegate}});
    }

    for(const auto& [uuid, node] : definition_pb.nodes()) {
        node_definitions_.insert({types::Uuid{uuid}, Node{node}});
    }

    for(const auto& [uuid, cluster] : definition_pb.clusters()) {
        cluster_definitions_.insert({types::Uuid{uuid}, Cluster{cluster}});
    }

    for(const auto& [uuid, trigger_definition] : definition_pb.trigger_definitions()) {
        trigger_definitions_.insert({types::Uuid{uuid}, TriggerDefinition{trigger_definition}});
    }

    for(const auto& [uuid, trigger_instance] : definition_pb.trigger_instances()) {
        trigger_instances_.insert({types::Uuid{uuid}, TriggerInstance{trigger_instance}});
    }

    for(const auto& [uuid, strategy] : definition_pb.strategies()) {
        strategies_.insert({types::Uuid{uuid}, Strategy{strategy}});
    }

    for(const auto& [uuid, logging_server] : definition_pb.logging_server_definitions()) {
        logging_server_definitions_.insert(
            {types::Uuid{uuid}, LoggingServerDefinition{logging_server}});
    }

    for(const auto& [uuid, logging_client] : definition_pb.logging_client_definitions()) {
        logging_client_definitions_.insert(
            {types::Uuid{uuid}, LoggingClientDefinition{logging_client}});
    }
}

} // namespace re::Representation
