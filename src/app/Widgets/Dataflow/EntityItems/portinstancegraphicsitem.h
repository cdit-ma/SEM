#ifndef PORTINSTGRAPHICSITEM_H
#define PORTINSTGRAPHICSITEM_H

#include "../../Charts/Data/portinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QPen>

class PortInstanceGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    PortInstanceGraphicsItem(const PortInstanceData& port_data, QGraphicsItem* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getPortName() const;
    AggServerResponse::Port::Kind getPortKind() const;

    QRectF getIconSceneRect() const;

    qint64 getPreviousEventTime(qint64 time) const;
    qint64 getNextEventTime(qint64 time) const;

    void setAlignment(Qt::Alignment alignment);

    void playEvents(qint64 from_time, qint64 to_time);

signals:
    void itemMoved();
    void flashEdge(qint64 from_time, int flash_duration_ms);

private slots:
    void flashPort(MEDEA::ChartDataKind event_kind, qint64 from_time, QColor flash_color = QColor());
    void unflashPort(MEDEA::ChartDataKind event_kind, qint64 flash_end_time);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void themeChanged();
    void setupCentralisedIconLayout();

    // This is used to prevent the flash from being stopped/reset prematurely due to previous flash timers ending
    qint64 flash_end_time_ = 0;
    bool event_src_port_ = false;

    Qt::Alignment alignment_ = Qt::AlignLeft;
    QPair<QString, QString> icon_path;

    qint64 port_lifecycle_flash_end_time_ = 0;
    qint64 port_event_flash_end_time_ = 0;

    QPen ellipse_pen_;
    QColor ellipse_pen_color_;

    QColor ellipse_color_;
    QColor default_color_;
    QColor highlight_color_;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* top_layout_ = nullptr;
    QGraphicsLinearLayout* children_layout_ = nullptr;

    PixmapGraphicsItem* icon_pixmap_item_ = nullptr;
    TextGraphicsItem* label_text_item_ = nullptr;

    PixmapGraphicsItem* sub_icon_pixmap_item_ = nullptr;
    TextGraphicsItem* sub_label_text_item_ = nullptr;

    const PortInstanceData& port_inst_data_;
};

#endif // PORTINSTGRAPHICSITEM_H
