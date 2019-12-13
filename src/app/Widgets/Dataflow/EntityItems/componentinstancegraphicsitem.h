#ifndef COMPINSTGRAPHICSITEM_H
#define COMPINSTGRAPHICSITEM_H

#include "../../Charts/Data/componentinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "portinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include <vector>

class ComponentInstanceGraphicsItem : public QGraphicsWidget
{    
    Q_OBJECT

public:
    ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, QGraphicsItem* parent = nullptr);

    PortInstanceGraphicsItem* addPortInstanceItem(PortInstanceData& port_data);

signals:
    void itemExpanded(bool expanded);
    void itemMoved();

protected:    
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    void moveEvent(QGraphicsSceneMoveEvent *event) override;
    
private:
    //void toggleDetailsVisible();
    void toggleExpanded();

    qreal getWidth() const;
    qreal getHeight() const;

    void themeChanged();
    void setupLayout();

    bool moving_ = false;
    bool expanded_ = true;
    //bool details_shown_ = true;

    int left_port_count_ = 0;
    int right_port_count_ = 0;

    QPointF prev_move_origin_;

    QColor top_color_;
    QColor body_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsGridLayout* children_layout_ = nullptr;

    PixmapGraphicsItem* toggle_pixmap_item_ = nullptr;
    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    std::vector<PortInstanceGraphicsItem*> port_inst_items_;
    const ComponentInstanceData& comp_inst_data_;
};

#endif // COMPINSTGRAPHICSITEM_H
