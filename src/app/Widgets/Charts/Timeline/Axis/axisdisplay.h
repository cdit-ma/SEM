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
    explicit AxisDisplay(const AxisSlider &slider, VALUE_TYPE type = VALUE_TYPE::DOUBLE, QWidget* parent = nullptr);

    void setAxisMargin(int margin);
    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);
    void setTickCount(int ticks);

    void setMax(double max);
    void setMin(double min);
    void setRange(double min, double max);

    QPair<double, double> getRange();
    QPair<double, double> getDisplayedRange();

    void setDisplayFormat(TIME_DISPLAY_FORMAT format);

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
    void paintHorizontal(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect);
    void paintVertical(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect);

    void displayedMinChanged(double min);
    void displayedMaxChanged(double max);
    void rangeChanged();

    QString getDateTimeString(double value);
    QString getElapsedTimeString(double value);
    QString getCovertedString(double value);
    QRectF getAdjustedRect();

    const AxisSlider& slider_;

    Qt::Orientation orientation_;
    Qt::Alignment textAlignment_;

    VALUE_TYPE valueType_;
    TIME_DISPLAY_FORMAT axisFormat_;

    QString currentDisplayFormat_ = TIME_FORMAT;
    bool displayFormatChanged_ = false;

    bool displayHoverValue_ = false;
    double hoveredValue_;
    double hoveredPos_;

    bool axisLineVisible_ = true;
    bool tickVisible_ = true;
    double tickLength_ = 4;
    int tickCount_ = 2;
    int penWidth_ = 4;

    double min_ = 0;
    double max_ = 100;
    double range_ = 100;

    double displayedMin_ = min_;
    double displayedMax_ = max_;
    double displayedRange_ = range_;

    int spacing_;
    int textHeight_;
    int widestTextLength_;

    QLineF axisLine_;
    QColor labelColor_;
    QColor axisColor_;
    QColor hoveredRectColor_;
    QColor hoverTextColor_;
};

#endif // AXISDISPLAY_H
