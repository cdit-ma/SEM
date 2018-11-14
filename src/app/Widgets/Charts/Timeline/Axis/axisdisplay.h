#ifndef AXISDISPLAY_H
#define AXISDISPLAY_H

#include "axisslider.h"
#include "../Chart/timelinechartview.h"

#include <QWidget>

class AxisDisplay : public QWidget
{
    friend class AxisWidget;
    Q_OBJECT

public:
    explicit AxisDisplay(AxisSlider* slider, QWidget* parent = 0, VALUE_TYPE type = VALUE_TYPE::DOUBLE);

    void setAxisMargin(int margin);
    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);
    void setTickCount(int ticks);

    // these functions update the displayed values
    /*
    void setMin(double min, bool setDisplay = false);
    void setMax(double max, bool setDisplay = false);
    void setRange(double min, double max, bool setDisplay = false);

    void setDisplayedMin(double min);
    void setDisplayedMax(double max);
    */

    void setMax(double max);
    void setMin(double min);
    void setRange(double min, double max);

    QPair<double, double> getRange();
    QPair<double, double> getDisplayedRange();

public slots:
    void hoverLineUpdated(bool visible, QPointF globalPos = QPointF());

private slots:
    void themeChanged();

    // these functions are called when the axis slider(s) are moved
    void updateDisplayedMin(double minRatio);
    void updateDisplayedMax(double maxRatio);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void paintHorizontal(QPainter &painter, QVector<QLineF> &tickLines, QRectF rect);
    void paintVertical(QPainter &painter, QVector<QLineF> &tickLines, QRectF rect);

    void displayedMinChanged(double min);
    void displayedMaxChanged(double max);
    void rangeChanged();

    double getValueAtTick(double index);
    QString getCovertedString(double value);
    QRectF getAdjustedRect();

    QDateTime constructDateTime(qint64 mSecsSinceEpoch);

    AxisSlider* _slider = 0;
    Qt::Orientation _orientation;
    Qt::Alignment _textAlignment;
    VALUE_TYPE _valueType;

    bool _displayHoverValue = false;
    double _hoveredValue;
    double _hoveredPos;

    bool _axisLineVisible = false;
    bool _tickVisible = true;
    double _tickLength = 4;
    int _tickCount = 2;
    int _penWidth = 4;

    double _min = 0;
    double _max = 100;
    double _range = 100;

    double _displayedMin = _min;
    double _displayedMax = _max;
    double _displayedRange = _range;

    int _spacing;
    int _textHeight;
    int _widestTextLength;
    int _displayedMinTextWidth;
    int _displayedMaxTextWidth;

    QLineF _axisLine;
    QColor _labelColor;
    QColor _axisColor;
    QColor _hoveredRectColor;
    QColor _hoverTextColor;
};

#endif // AXISDISPLAY_H
