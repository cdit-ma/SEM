//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_DEFAULTENTITY_H
#define PULSE_VIEW_DEFAULTENTITY_H

#include "entity.h"
#include "../NamePlate/nameplate.h"

#include <QPointer>

namespace Pulse::View {

class DefaultEntity : public QGraphicsWidget, public Entity {
public:
    explicit DefaultEntity(const QString& label,
                           const QString& icon_path,
                           const QString& icon_name,
                           const QString& meta_label,
                           const QString& meta_icon_path,
                           const QString& meta_icon_name,
                           QGraphicsItem* parent = nullptr);

    ~DefaultEntity() override = default;

    void connectModelData(QPointer<Pulse::Model::Entity> model_data) override;
    void onModelDeleted() override;

private:
    NamePlate* name_plate_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTENTITY_H