#ifndef PORTINSTGRAPHICSITEM_H
#define PORTINSTGRAPHICSITEM_H

#include "../../Charts/Data/portinstancedata.h"
#include "../GraphicsLayoutItems/pixmapgraphicsitem.h"
#include "../GraphicsLayoutItems/textgraphicsitem.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

class PortInstanceGraphicsItem : public QGraphicsWidget
{
    Q_OBJECT

public:
    PortInstanceGraphicsItem(const PortInstanceData& port_data, QGraphicsItem* parent = nullptr);

    const QString& getGraphmlID() const;
    const QString& getPortName() const;
    AggServerResponse::Port::Kind getPortKind() const;

    QRectF getIconSceneRect() const;

    void setAlignment(Qt::Alignment alignment);

    void flashPort(qint64 start_time, qint64 flash_duration_ms, QColor flash_color = QColor());

signals:
    void itemMoved();

private slots:
    void unflashPort(qint64 flash_end_time);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void themeChanged();
    void setupCentralisedIconLayout();

    // This is used to prevent the flash from being stopped/reset prematurely due to previous flash timers ending
    qint64 flash_end_time_ = 0;

    Qt::Alignment alignment_ = Qt::AlignLeft;
    QPair<QString, QString> icon_path;

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

    QString graphml_id_;
    QString port_name_;
    AggServerResponse::Port::Kind port_kind_;
};

#endif // PORTINSTGRAPHICSITEM_H
