#ifndef NODEDATA_H
#define NODEDATA_H

#include "protomessagestructs.h"
#include "containerinstancedata.h"

#include "Requests/hardwaremetricrequest.h"
#include "Events/cpuutilisationevent.h"
#include "Events/memoryutilisationevent.h"
#include "Events/networkutilisationevent.h"
#include "Events/gpucomputeutilisationevent.h"
#include "Events/gpumemoryutilisationevent.h"
#include "Events/gputemperatureevent.h"
#include "Series/cpuutilisationeventseries.h"
#include "Series/memoryutilisationeventseries.h"
#include "Series/networkutilisationeventseries.h"
#include "Series/gpucomputeutilisationseries.h"
#include "Series/gpumemoryutilisationseries.h"
#include "Series/gputemperatureseries.h"

class NodeData : public QObject
{
    Q_OBJECT

public:
    NodeData(quint32 exp_run_id, const AggServerResponse::Node &node, QObject *parent = nullptr);

    const QString& getHostname() const;
    const QString& getIP() const;

    QList<ContainerInstanceData*> getContainerInstanceData() const;

    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QString& port_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortLifecycleSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QString& port_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getPortEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& port_paths) const;

    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QString& worker_inst_path = "") const;
    QList<QPointer<const MEDEA::EventSeries>> getWorkloadEventSeries(const QStringList& comp_names, const QStringList& comp_inst_paths, const QStringList& worker_inst_paths) const;

    const HardwareMetricRequest& getCPUUtilisationRequest() const;
    const HardwareMetricRequest& getMemoryUtilisationRequest() const;
    const HardwareMetricRequest& getNetworkUtilisationRequest() const;
    const HardwareMetricRequest& getGPUMetricsRequest() const;

    void addCPUUtilisationEvents(const QVector<CPUUtilisationEvent*>& events);
    QPointer<const MEDEA::EventSeries> getCPUUtilisationSeries() const;

    void addMemoryUtilisationEvents(const QVector<MemoryUtilisationEvent*>& events);
    QPointer<const MEDEA::EventSeries> getMemoryUtilisationSeries() const;

    void addNetworkUtilisationEvents(const QVector<NetworkUtilisationEvent*>& events);
    QList<QPointer<const MEDEA::EventSeries>> getNetworkUtilisationSeries() const;

    void addGPUComputeUtilisationEvents(const QVector<GPUComputeUtilisationEvent *> &events);
    QList<QPointer<const MEDEA::EventSeries>> getGPUComputeUtilisationSeries() const;

    void addGPUMemoryUtilisationEvents(const QVector<GPUMemoryUtilisationEvent *> &events);
    QList<QPointer<const MEDEA::EventSeries>> getGPUMemoryUtilisationSeries() const;

    void addGPUTemperatureEvents(const QVector<GPUTemperatureEvent *> &events);
    QList<QPointer<const MEDEA::EventSeries>> getGPUTemperatureSeries() const;

    void updateData(const AggServerResponse::Node& node, qint64 new_last_updated_time);

signals:
    void requestData(NodeData& node);

private:
    void addContainerInstanceData(const AggServerResponse::Container& container);

    void setupRequests();
    void setupSeries(const QVector<AggServerResponse::NetworkInterface>& interfaces,
                     const QVector<AggServerResponse::GPUDevice>& gpu_devices);

    quint32 experiment_run_id_;
    qint64 last_updated_time_;

    QString hostname_;
    QString ip_;

    QHash<QString, ContainerInstanceData *> container_inst_data_hash_;

    HardwareMetricRequest cpu_utilisation_request_;
    HardwareMetricRequest memory_utilisation_request_;
    HardwareMetricRequest network_utilisation_request_;
    HardwareMetricRequest gpu_metrics_request_;

    CPUUtilisationEventSeries* cpu_utilisation_series_ = nullptr;
    MemoryUtilisationEventSeries* memory_utilisation_series_ = nullptr;
    QHash<QString, NetworkUtilisationEventSeries*> network_utilisation_series_;

    QHash<QString, GPUComputeUtilisationSeries*> gpu_compute_utilisation_series_;
    QHash<QString, GPUMemoryUtilisationSeries*> gpu_memory_utilisation_series_;
    QHash<QString, GPUTemperatureSeries*> gpu_temperature_series_;
};

#endif // NODEDATA_H