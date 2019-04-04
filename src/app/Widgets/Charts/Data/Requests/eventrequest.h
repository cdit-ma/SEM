#ifndef EVENTREQUEST_H
#define EVENTREQUEST_H

#include "request.h"

class EventRequest : public Request {

protected:
    explicit EventRequest(MEDEA::ChartDataKind kind);

public:
    void setComponentNames(const QVector<QString>& component_names);
    void setComponentInstanceIDS(const QVector<QString>& component_instance_ids);
    void setComponentInstancePaths(const QVector<QString>& component_instance_paths);

    const QVector<QString>& component_names() const;
    const QVector<QString>& component_instance_ids() const;
    const QVector<QString>& component_instance_paths() const;

private:
    QVector<QString> componentNames_;
    QVector<QString> componentInstanceIDs_;
    QVector<QString> componentInstancePaths_;

};

#endif // EVENTREQUEST_H
