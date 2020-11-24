//
// Created by Cathlyn Aston on 5/11/20.
//

#include "portinstance.h"
#include "../pulseviewutils.h"
#include "../../../theme.h"

#include <QPainter>

using namespace Pulse::View;

/**
 * @brief PortInstance::PortInstance
 * @param label
 * @param kind
 * @param meta_label
 * @param parent
 */
PortInstance::PortInstance(const QString& label,
                           const AggServerResponse::Port::Kind kind,
                           const QString& meta_label,
                           QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      name_plate_(new NamePlate(label, "Icons", Utils::getIconName(kind),
                                meta_label, "Icons", "envelopeTwoTone",
                                this)),
      kind_(kind)
{
    auto icon_pos = NamePlate::IconPos::Left;
    switch (kind) {
        case AggServerResponse::Port::Kind::PERIODIC:
            break;
        case AggServerResponse::Port::Kind::SUBSCRIBER:
            [[fallthrough]];
        case AggServerResponse::Port::Kind::REPLIER:
            input_anchor_ = new NaturalAnchor(this);
            break;
        case AggServerResponse::Port::Kind::PUBLISHER:
            [[fallthrough]];
        case AggServerResponse::Port::Kind::REQUESTER:
            icon_pos = NamePlate::IconPos::Right;
            output_anchor_ = new NaturalAnchor(this);
            break;
        case AggServerResponse::Port::Kind::NO_KIND:
            throw std::invalid_argument("PortInstanceGraphicsItem::PortInstanceGraphicsItem - Port kind is unknown");
    }

    name_plate_->setIconPos(icon_pos);
    name_plate_->setPrimarySpacing(Defaults::layout_padding);

    auto icon_size = Defaults::primary_icon_size * 0.75;
    name_plate_->setPrimaryIconSize(icon_size.width(), icon_size.height());

    auto layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(name_plate_);

    const auto& icon_geom = name_plate_->getIconGeometry();
    if (input_anchor_ != nullptr) {
        input_anchor_->setPos(icon_geom.left(), icon_geom.center().y());
    }
    if (output_anchor_ != nullptr) {
        output_anchor_->setPos(icon_geom.right(), icon_geom.center().y());
    }

    /*
    ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::ERROR);
    if (icon_pos == NamePlate::IconPos::Right) {
        ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::SUCCESS);
    }
     */

    auto theme_changed = [icon_pos, this]() {
        ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::ERROR);
        if (icon_pos == NamePlate::IconPos::Right) {
            ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::SUCCESS);
        }
        update();
    };

    connect(Theme::theme(), &Theme::theme_Changed, [=]() { theme_changed(); });

    /*
        // Connect the theme to update the ellipse color
    connect(Theme::theme(), &Theme::theme_Changed, [icon_pos, this]() {
        ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::ERROR);
        if (icon_pos == NamePlate::IconPos::Right) {
            ellipse_color_ = Theme::theme()->getSeverityColor(Notification::Severity::SUCCESS);
        }
        update();
    });*/

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    theme_changed();
}

/**
 * @brief PortInstance::connectModelData
 * @param model_data
 */
void PortInstance::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("DefaultEntity - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &PortInstance::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::labelChanged, name_plate_, &NamePlate::changeLabel);
    connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
}

/**
 * @brief PortInstance::onModelDeleted
 */
void PortInstance::onModelDeleted()
{
    deleteLater();
}


void PortInstance::connectEdge(Edge* edge)
{
    input_anchor_->connectEdge(edge);
}

void PortInstance::disconnectEdges()
{
    input_anchor_->disconnectEdges();
}

NaturalAnchor* PortInstance::getInputAnchor()
{
    return input_anchor_;
}


NaturalAnchor* PortInstance::getOutputAnchor()
{
    return output_anchor_;
}

/**
 * @brief PortInstance::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* PortInstance::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief PortInstance::getKind
 * @return
 */
AggServerResponse::Port::Kind PortInstance::getKind() const
{
    return kind_;
}

/**
 * @brief PortInstance::boundingRect
 * @return
 */
QRectF PortInstance::boundingRect() const
{
    return name_plate_->boundingRect();
}

/**
 * @brief PortInstance::paint
 * @param painter
 * @param option
 * @param widget
 */
void PortInstance::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(ellipse_color_);

    auto ellipse_geom = name_plate_->getIconGeometry();
    painter->drawEllipse(ellipse_geom.center(), ellipse_geom.height() / 2.0, ellipse_geom.height() / 2.0);
}