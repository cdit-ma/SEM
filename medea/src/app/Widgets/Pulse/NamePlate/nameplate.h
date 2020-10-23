//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_NAMEPLATE_H
#define PULSE_VIEW_NAMEPLATE_H

#include "entityinfodisplay.h"
#include "textitem.h"
#include "pixmapitem.h"

#include <QGraphicsWidget>

namespace Pulse::View {

class NamePlate : QGraphicsWidget, EntityInfoDisplay {
public:
    explicit NamePlate(const QString& label,
                       const QString& icon_name,
                       const QString& meta_label,
                       const QString& meta_icon_name,
                       QGraphicsItem* parent = nullptr);
    ~NamePlate() = default;

    void changeIcon(const QString& icon_name) override;
    void changeLabel(const QString& label) override;

private:
    TextItem* text_item_ = nullptr;
    PixmapItem* pixmap_item_ = nullptr;

    TextItem* meta_text_item_ = nullptr;
    PixmapItem* meta_pixmap_item_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_NAMEPLATE_H
