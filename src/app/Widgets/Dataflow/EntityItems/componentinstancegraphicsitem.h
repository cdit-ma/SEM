#ifndef COMPINSTGRAPHICSITEM_H
#define COMPINSTGRAPHICSITEM_H

#include "../../Charts/Data/componentinstancedata.h"
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
    ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, QGraphicsItem* parent = nullptr);

    PortInstanceGraphicsItem* addPortInstanceItem(PortInstanceData& port_data);

    //void update(qint64 from_time, qint64 to_time);

signals:
    void toggleExpanded(bool expand);
    void itemMoved();

protected:    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    void moveEvent(QGraphicsSceneMoveEvent *event) override;
    
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

    const ComponentInstanceData& comp_inst_data_;
};

#endif // COMPINSTGRAPHICSITEM_H
