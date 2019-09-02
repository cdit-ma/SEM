#ifndef PORTINSTGRAPHICSITEM_H
#define PORTINSTGRAPHICSITEM_H

#include "../../../Widgets/Charts/Data/Events/protomessagestructs.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

class PortInstanceGraphicsItem : public QGraphicsWidget
{
public:
    PortInstanceGraphicsItem(const AggServerResponse::Port& port, QGraphicsItem* parent = nullptr);

    AggServerResponse::Port::Kind getPortKind() const;
    void setAlignment(Qt::Alignment alignment);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    void themeChanged();
    void setupCentralisedIconLayout();

    Qt::Alignment alignment_ = Qt::AlignLeft;
    QPair<QString, QString> icon_path;
    QColor ellipse_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsLinearLayout* children_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    PixmapGraphicsItem* sub_icon_pixmap_item_ = nullptr;
    TextGraphicsItem* sub_label_text_item_ = nullptr;

    const AggServerResponse::Port& port_;
    AggServerResponse::Port::Kind port_kind_;
};

#endif // PORTINSTGRAPHICSITEM_H
