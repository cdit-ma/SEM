//
// Created by Cathlyn Aston on 24/9/20.
//

#include "defaultentity.h"

#include <stdexcept>

using namespace Pulse::View;

/**
 * @brief DefaultEntity::DefaultEntity
 * @param label
 * @param icon_path
 * @param icon_name
 * @param meta_label
 * @param meta_icon_path
 * @param meta_icon_name
 * @param parent
 */
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

    setFlags(flags() | QGraphicsWidget::ItemIsMovable);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

/**
 * @brief DefaultEntity::connectModelData
 * @param model_data
 * @throws std::invalid_argument
 */
void DefaultEntity::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("DefaultEntity::connectModelData - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &DefaultEntity::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::nameChanged, name_plate_, &NamePlate::changeName);
}

/**
 * @brief DefaultEntity::onModelDeleted
 */
void DefaultEntity::onModelDeleted()
{
    deleteLater();
}

/**
 * @brief DefaultEntity::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* DefaultEntity::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief DefaultEntity::boundingRect
 * @return
 */
QRectF DefaultEntity::boundingRect() const
{
    return name_plate_->boundingRect();
}

/**
 * @brief DefaultEntity::setPrimaryIconSize
 * @param width
 * @param height
 */
void DefaultEntity::setPrimaryIconSize(int width, int height)
{
    name_plate_->setPrimaryIconSize(width, height);
}

/**
 * @brief DefaultEntity::itemChange
 * @param change
 * @param value
 * @return
 */
QVariant DefaultEntity::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (parentItem()) {
        if (change == ItemPositionChange) {
            auto new_pos = value.toPoint();
            new_pos.setX(qMax(0, new_pos.x()));
            new_pos.setY(qMax(0, new_pos.y()));
            return new_pos;
        }
    }
    return QGraphicsWidget::itemChange(change, value);
}