#ifndef NODEGRAPHICSITEM_H
#define NODEGRAPHICSITEM_H

#include "../../Charts/Data/nodedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "containerinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <vector>

class NodeGraphicsItem : public QGraphicsWidget
{    
    Q_OBJECT

public:
    explicit NodeGraphicsItem(const NodeData& node_data, QGraphicsItem* parent = nullptr);

    void addContainerInstanceItem(ContainerInstanceData& container_inst_data);
    const std::vector<ContainerInstanceGraphicsItem*>& getContainerInstanceItems() const;

signals:
    void updateConnectionPos();

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
    void validateChildMove(ContainerInstanceGraphicsItem* child, QPointF pos);

    qreal getWidth() const;
    qreal getHeight() const;

    QRectF getTopRect() const;
    QRectF getVisibleChildrenRect() const;

    // This returns the next available pos vertically where a child can be placed without being put on top of another
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

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    std::vector<ContainerInstanceGraphicsItem*> container_inst_items_;
    const NodeData& node_data_;
};

#endif // NODEGRAPHICSITEM_H