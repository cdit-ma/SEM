#ifndef EXPERIMENTRUNDATA_H
#define EXPERIMENTRUNDATA_H

#include "Events/portlifecycleevent.h"
#include "Series/portlifecycleeventseries.h"

namespace MEDEA {

class ExperimentRunData {

public:

    // Passkey class is used in the constructor to guard against unknown classes
    // calling it (ie only friends of PassKey can construct it, and can therefore
    // call the constructor)
    class PassKey {
    private:
        // List of friend classes that may make the passkey
        friend class ExperimentData;

        // Creates an empty PassKey with no extra bells or whistles
        PassKey() = default;

        //We need to be able to create a copy *in-place* from another PassKey reference
        PassKey(const PassKey&) = default;

        // We don't want anyone being able to store them anywhere, we want total control!
        PassKey& operator=(const PassKey&) = delete;
    };

    ExperimentRunData(PassKey,
                      quint32 experiemnt_run_id,
                      quint32 job_num,
                      qint64 start_time,
                      qint64 end_time = 0,
                      qint64 last_updated_time = 0);

    quint32 experiment_run_id() const;
    quint32 job_num() const;

    qint64 start_time() const;
    qint64 end_time() const;
    qint64 last_updated_time() const;

    const QVector<AggServerResponse::Node>& nodes() const;
    const QVector<AggServerResponse::Component>& components() const;
    const QVector<AggServerResponse::Worker>& workers() const;
    const QVector<AggServerResponse::PortConnection>& port_connections() const;

    bool hasState() const;
    void setExperimentState(const AggServerResponse::ExperimentState& exp_state);

    void updateEndTime(qint64 time);
    void updateLastUpdatedTime(qint64 time);

    // Stubs for the new data classes
    void addNodeData();
    void addContainerInstanceData();
    void addComponentInstanceData();
    void addPortInstanceData();
    void addWorkerInstanceData();

    void addPortConnections();
    void addMarkers();

    // TODO - Implement helper functions for getting specific series (include filters)

private:
    quint32 experiment_run_id_;
    quint32 job_num_;
    qint64 start_time_;

    qint64 end_time_;
    qint64 last_updated_time_;

    bool has_state_ = false;

    // Experiment State Vectors
    QVector<AggServerResponse::Node> nodes_;
    QVector<AggServerResponse::Component> components_;
    QVector<AggServerResponse::Worker> workers_;
    QVector<AggServerResponse::PortConnection> port_connections_;

};

}

#endif // EXPERIMENTRUNDATA_H
