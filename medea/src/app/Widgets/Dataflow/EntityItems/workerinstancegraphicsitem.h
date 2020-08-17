//
// Created by Cathlyn Aston on 17/8/20.
//

#ifndef SEM_WORKERINSTANCEGRAPHICSITEM_H
#define SEM_WORKERINSTANCEGRAPHICSITEM_H

#include "../../Charts//Data//workerinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

class ComponentInstanceGraphicsItem;

class WorkerInstanceGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    WorkerInstanceGraphicsItem(const WorkerInstanceData& worker_data, ComponentInstanceGraphicsItem* parent);

    const QString& getGraphmlID() const;
    const QString& getName() const;
    const QString& getType() const;

protected:
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    qreal getWidth() const;
    qreal getHeight() const;

    void themeChanged();
    void setupLayout();

    QColor active_color_;
    QColor default_color_;
    QColor highlight_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    ComponentInstanceGraphicsItem* parent_comp_inst_item_ = nullptr;
    const WorkerInstanceData& worker_inst_data_; // TODO: Ask if this is okay
};

#endif // SEM_WORKERINSTANCEGRAPHICSITEM_H