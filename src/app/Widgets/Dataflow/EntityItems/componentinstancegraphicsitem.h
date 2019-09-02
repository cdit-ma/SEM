#ifndef COMPINSTGRAPHICSITEM_H
#define COMPINSTGRAPHICSITEM_H

#include "../../../Widgets/Charts/Data/Events/protomessagestructs.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "portinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

class ComponentInstanceGraphicsItem : public QGraphicsWidget
{    
    Q_OBJECT

public:
    ComponentInstanceGraphicsItem(const AggServerResponse::ComponentInstance& comp_inst, QGraphicsItem* parent = nullptr);

    void addPortInstanceItem(PortInstanceGraphicsItem* item);

signals:
    void toggleExpanded(bool expand);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
private:
    void themeChanged();
    void setupLayout();

    int left_port_count_ = 0;
    int right_port_count_ = 0;

    bool moving_ = false;

    QPointF prev_move_origin_;

    QColor top_color_;
    QColor body_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsGridLayout* children_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;  

    AggServerResponse::ComponentInstance comp_instance_;

};

#endif // COMPINSTGRAPHICSITEM_H
