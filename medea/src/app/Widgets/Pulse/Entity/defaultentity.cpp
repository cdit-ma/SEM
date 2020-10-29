//
// Created by Cathlyn Aston on 24/9/20.
//

#include "defaultentity.h"
using namespace Pulse::View;

DefaultEntity::DefaultEntity(const QString& label,
                                          const QString& icon_path,
                                          const QString& icon_name,
                                          const QString& meta_label,
                                          const QString& meta_icon_path,
                                          const QString& meta_icon_name,
                                          QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      name_plate_(new NamePlate(label, icon_path, icon_name, meta_label, meta_icon_path, meta_icon_name, this))
{
    auto layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(name_plate_);
}

void DefaultEntity::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (!model_data.isNull()) {
        connect(model_data, &Pulse::Model::Entity::destroyed, this, &DefaultEntity::onModelDeleted);
        connect(model_data, &Pulse::Model::Entity::labelChanged, name_plate_, &NamePlate::changeLabel);
        connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
    }
}

void DefaultEntity::onModelDeleted()
{
    deleteLater();
}

