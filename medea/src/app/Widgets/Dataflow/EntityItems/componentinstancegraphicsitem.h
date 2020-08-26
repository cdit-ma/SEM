#ifndef COMPONENTINSTGRAPHICSITEM_H
#define COMPONENTINSTGRAPHICSITEM_H

#include "../../Charts/Data/componentinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"
#include "portinstancegraphicsitem.h"
#include "workerinstancegraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include <vector>

class ContainerInstanceGraphicsItem;

class ComponentInstanceGraphicsItem : public QGraphicsWidget
{    
    Q_OBJECT

public:
    explicit ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, ContainerInstanceGraphicsItem* parent = nullptr);

    const QString& getGraphmlID() const;

    void addPortInstanceItem(PortInstanceData& port_data);
    const std::vector<PortInstanceGraphicsItem*>& getPortInstanceItems() const;

    void addWorkerInstanceItem(WorkerInstanceData& worker_inst_data);
    const std::vector<WorkerInstanceGraphicsItem*>& getWorkerInstanceItems() const;

signals:
    void updateConnectionPos();
    void requestMove(ComponentInstanceGraphicsItem* child, QPointF pos);

protected:    
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void constructChildrenItems();
    void toggleExpanded();

    qreal getWidth() const;
    qreal getHeight() const;

    void themeChanged();
    void setupLayout();

    bool moving_ = false;
    bool expanded_ = true;

    int left_port_count_ = 0;
    int right_port_count_ = 0;
    int worker_count = 0;

    QPointF prev_move_origin_;

    QColor top_color_;
    QColor body_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsGridLayout* children_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    const ComponentInstanceData& comp_inst_data_;
    std::vector<PortInstanceGraphicsItem*> port_inst_items_;
    std::vector<WorkerInstanceGraphicsItem*> worker_inst_items_;
};

#endif // COMPONENTINSTGRAPHICSITEM_H