//
// Created by Cathlyn Aston on 26/10/20.
//

#ifndef PULSE_VIEW_DEFAULTS_H
#define PULSE_VIEW_DEFAULTS_H

#include <QFont>

namespace Pulse::View::Defaults {

constexpr qreal pen_width = 2.0;

constexpr int layout_margin = 5;
constexpr int layout_padding = 10;

static inline const QSize primary_icon_size(50, 50);
static inline const QSize secondary_icon_size = primary_icon_size / 3.5;

static inline const QFont primary_font("Verdana", 10);
static inline const QFont secondary_font("Verdana", 6);

static inline const QColor input_port_color(244, 0, 0);
static inline const QColor output_port_color(0, 214, 0);

} // end Pulse::View::Defaults namespace
#endif // PULSE_VIEW_DEFAULTS_H
