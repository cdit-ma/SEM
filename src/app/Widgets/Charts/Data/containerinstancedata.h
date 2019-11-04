#ifndef CONTAINERINSTANCEDATA_H
#define CONTAINERINSTANCEDATA_H

#include "protomessagestructs.h"
#include "componentinstancedata.h"

class ContainerInstanceData {

public:
    ContainerInstanceData(const AggServerResponse::Container& container);

    const QString& getGraphmlID() const;
    const QString& getName() const;

    AggServerResponse::Container::ContainerType getType() const;

    void addComponentInstanceData(const AggServerResponse::ComponentInstance& comp_inst);
    QList<ComponentInstanceData> getComponentInstanceData() const;

private:
    QString graphml_id_;
    QString name_;

    AggServerResponse::Container::ContainerType type_;

    QHash<QString, ComponentInstanceData> comp_inst_data_hash_;
};

#endif // CONTAINERINSTANCEDATA_H
