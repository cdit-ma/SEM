//
// Created by Cathlyn Aston on 20/8/20.
//

#ifndef CONTAINERINSTGRAPHICSITEM_H
#define CONTAINERINSTGRAPHICSITEM_H

#include "../../Charts/Data/containerinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "componentinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include <vector>

class NodeGraphicsItem;

class ContainerInstanceGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit ContainerInstanceGraphicsItem(const ContainerInstanceData& container_inst_data, NodeGraphicsItem* parent = nullptr);

    void addComponentInstanceItem(ComponentInstanceData& comp_inst_data);
    const std::vector<ComponentInstanceGraphicsItem*>& getComponentInstanceItems() const;

signals:
    void updateConnectionPos();
    void requestMove(ContainerInstanceGraphicsItem* child, QPointF pos);

protected:
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;
    void setGeometry(const QRectF &rect) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void constructChildrenItems();
    void toggleExpanded();
    void validateChildMove(ComponentInstanceGraphicsItem* child, const QPointF& pos);

    qreal getWidth() const;
    qreal getHeight() const;

    QRectF getTopRect() const;
    QRectF getVisibleChildrenRect() const;

    QPointF getNextChildPos() const;
    QPointF getOriginChildPos() const;

    void themeChanged();
    void setupLayout();

    bool moving_ = false;
    bool expanded_ = true;

    QPointF prev_move_origin_;

    QColor top_color_;
    QColor body_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsLinearLayout* text_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;
    TextGraphicsItem* metadata_text_item_ = nullptr;

    std::vector<ComponentInstanceGraphicsItem*> comp_inst_items_;
    const ContainerInstanceData& container_inst_data_;
};

#endif // CONTAINERINSTGRAPHICSITEM_H