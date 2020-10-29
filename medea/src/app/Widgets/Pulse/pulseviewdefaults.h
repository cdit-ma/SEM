//
// Created by Cathlyn Aston on 26/10/20.
//

#ifndef PULSE_VIEW_DEFAULTS_H
#define PULSE_VIEW_DEFAULTS_H

#include <QFont>

namespace Pulse::View::Defaults {

constexpr int minimum_width = 170;
constexpr int minimum_height = 70;

constexpr int big_padding = 15;
constexpr int small_padding = 2;

constexpr int primary_layout_spacing = 10;
constexpr int secondary_layout_spacing = 5;
constexpr int tertiary_layout_spacing = 2;
constexpr int layout_margin = 5;

static inline const QSize primary_icon_size(40, 40);
static inline const QSize secondary_icon_size = primary_icon_size / 2.5;

static inline const QFont primary_font("Verdana", 10);
static inline const QFont secondary_font("Verdana", 6);

} // end Pulse::View::Defaults namespace
#endif // PULSE_VIEW_DEFAULTS_H
