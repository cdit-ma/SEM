//
// Created by Cathlyn Aston on 28/10/20.
//

#ifndef PULSE_VIEW_DEFAULTENTITYCONTAINER_H
#define PULSE_VIEW_DEFAULTENTITYCONTAINER_H

#include "entitycontainer.h"
#include "../Entity/entity.h"
#include "../NamePlate/nameplate.h"

#include <QPointer>

namespace Pulse::View {

class DefaultEntityContainer : public QGraphicsWidget, public Entity, public EntityContainer {
public:
    explicit DefaultEntityContainer(const QString& label,
                                    const QString& icon_path,
                                    const QString& icon_name,
                                    const QString& meta_label,
                                    const QString& meta_icon_path,
                                    const QString& meta_icon_name,
                                    QGraphicsItem* parent = nullptr);

    ~DefaultEntityContainer() override = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

    void add(QPointer<Entity> entity) override;
    void remove(QPointer<Entity> entity) override;

    void expand() override;
    void contract() override;

private:
    NamePlate* name_plate_ = nullptr;
    QGraphicsLinearLayout* children_layout_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTENTITYCONTAINER_H
