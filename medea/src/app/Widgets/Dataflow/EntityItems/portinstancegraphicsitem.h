#ifndef PORTINSTGRAPHICSITEM_H
#define PORTINSTGRAPHICSITEM_H

#include "../../Charts/Data/portinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QPen>

class ComponentInstanceGraphicsItem;

class PortInstanceGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    PortInstanceGraphicsItem(const PortInstanceData& port_data, ComponentInstanceGraphicsItem* parent);

    const QString& getGraphmlID() const;
    const QString& getPortName() const;
    AggServerResponse::Port::Kind getPortKind() const;

    QPointF getEdgePoint() const;

    qint64 getPreviousEventTime(qint64 time) const;
    qint64 getNextEventTime(qint64 time) const;
    void playEvents(qint64 from_time, qint64 to_time);

    void setAlignment(Qt::Alignment alignment);

signals:
    void updateConnectionPos();
    void flashEdge(qint64 from_time, int flash_duration_ms);

private slots:
    void flashPort(MEDEA::ChartDataKind event_kind, qint64 from_time, QColor flash_color = QColor());
    void unflashPort(MEDEA::ChartDataKind event_kind, qint64 flash_end_time);

protected:
    QRectF boundingRect() const override;
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint) const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    qreal getWidth() const;
    qreal getHeight() const;

    void themeChanged();
    void setupLayout();
    void setupSubInfoLayout();

    bool event_src_port_ = false;
    Qt::Alignment alignment_ = Qt::AlignLeft;

    QPair<QString, QString> icon_path;

    // These are used to prevent the flash from being stopped/reset prematurely due to previous flash timers ending
    qint64 port_lifecycle_flash_end_time_ = 0;
    qint64 port_event_flash_end_time_ = 0;

    QPen ellipse_pen_;
    QColor ellipse_pen_color_;

    QColor ellipse_color_;
    QColor default_color_;
    QColor highlight_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* info_layout_ = nullptr;
    QGraphicsLinearLayout* sub_info_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    PixmapGraphicsItem* sub_icon_pixmap_item_ = nullptr;
    TextGraphicsItem* sub_label_text_item_ = nullptr;

    ComponentInstanceGraphicsItem* parent_comp_inst_item_ = nullptr;
    const PortInstanceData& port_inst_data_;
};

#endif // PORTINSTGRAPHICSITEM_H