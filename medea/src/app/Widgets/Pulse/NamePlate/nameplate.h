//
// Created by Cathlyn Aston on 24/9/20.
//

#ifndef PULSE_VIEW_NAMEPLATE_H
#define PULSE_VIEW_NAMEPLATE_H

#include "entityinfodisplay.h"
#include "textitem.h"
#include "pixmapitem.h"
#include "../pulseviewdefaults.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

namespace Pulse::View {

class NamePlate : public QGraphicsWidget, public EntityInfoDisplay {
public:
    enum IconPos{Left, Right};

    explicit NamePlate(const QString& label,
                       const QString& icon_path,
                       const QString& icon_name,
                       const QString& meta_label,
                       const QString& meta_icon_path,
                       const QString& meta_icon_name,
                       QGraphicsItem* parent = nullptr);

    ~NamePlate() override = default;

    void changeIcon(const QString& icon_path, const QString& icon_name) override;
    void changeName(const QString& name) override;

    void setIconPos(IconPos pos);
    [[nodiscard]] QRectF getIconGeometry() const;

    void setPrimarySpacing(int spacing);
    void setPrimaryIconSize(int width, int height);

private:
    void themeChanged();
    void setupLayout();

    TextItem* primary_text_item_ = nullptr;
    PixmapItem* primary_pixmap_item_ = nullptr;
    QPair<QString, QString> primary_icon_;
    QSize primary_icon_size_ = Defaults::primary_icon_size;

    TextItem* secondary_text_item_ = nullptr;
    PixmapItem* secondary_pixmap_item_ = nullptr;
    QPair<QString, QString> secondary_icon_;

    IconPos current_icon_pos_ = IconPos::Left;

    QGraphicsLinearLayout* main_layout_ = nullptr;
    QGraphicsLinearLayout* info_layout_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_NAMEPLATE_H