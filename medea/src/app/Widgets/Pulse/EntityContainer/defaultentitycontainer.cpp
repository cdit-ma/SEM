//
// Created by Cathlyn Aston on 28/10/20.
//

#include "defaultentitycontainer.h"
using namespace Pulse::View;

DefaultEntityContainer::DefaultEntityContainer(const QString& label,
                                               const QString& icon_path,
                                               const QString& icon_name,
                                               const QString& meta_label,
                                               const QString& meta_icon_path,
                                               const QString& meta_icon_name,
                                               QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      name_plate_(new NamePlate(label, icon_path, icon_name, meta_label, meta_icon_path, meta_icon_name, this))
{
    auto top_layout = new QGraphicsLinearLayout;
    top_layout->setContentsMargins(0, 0, 0, 0);
    top_layout->addItem(name_plate_);

    children_layout_ = new QGraphicsLinearLayout;

    auto main_layout_ = new QGraphicsLinearLayout(Qt::Vertical, this);
    main_layout_->setSpacing(0);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->addItem(top_layout);

}

void DefaultEntityContainer::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (!model_data.isNull()) {
        connect(model_data, &Pulse::Model::Entity::destroyed, this, &DefaultEntityContainer::onModelDeleted);
        connect(model_data, &Pulse::Model::Entity::labelChanged, name_plate_, &NamePlate::changeLabel);
        connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
    }
}

void DefaultEntityContainer::onModelDeleted()
{
    deleteLater();
}

void DefaultEntityContainer::add(QPointer<Entity> entity)
{

}

void DefaultEntityContainer::remove(QPointer<Entity> entity) {

}

void DefaultEntityContainer::expand() {

}

void DefaultEntityContainer::contract() {

}


