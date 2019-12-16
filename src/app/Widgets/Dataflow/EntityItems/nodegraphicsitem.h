#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include "../../Charts/Data/nodedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "componentinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <vector>

class NodeGraphicsItem : public QGraphicsWidget
{    
    Q_OBJECT

public:
    NodeGraphicsItem(const NodeData& node_data, QGraphicsItem* parent = nullptr);

    ComponentInstanceGraphicsItem* addComponentInstanceItem(ComponentInstanceData& comp_inst_data);

signals:
    void updateConnectionPos();

protected:    
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;

    void setGeometry(const QRectF& rect) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    
private:
    void validateChildMove(ComponentInstanceGraphicsItem* child, QPointF pos);
    void toggleExpanded();
    void updateOnGeometryChange();

    qreal getWidth() const;
    qreal getHeight() const;

    QRectF getTopRect() const;
    QRectF getVisibleChildrenRect() const;

    QPointF getTopLeftChildPos() const;
    // REVIEW (Jackson): What is meant by available?
    QPointF getAvailableChildPos() const;

    void themeChanged();
    void setupLayout();

    bool moving_ = false;
    bool expanded_ = true;

    QPointF prev_move_origin_;

    QColor top_color_;
    QColor body_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    std::vector<ComponentInstanceGraphicsItem*> comp_inst_items_;
    const NodeData& node_data_;
};

#endif // NODEGRAPHICSITEM_H
