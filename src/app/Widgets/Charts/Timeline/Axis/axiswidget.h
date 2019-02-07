#ifndef AXISWIDGET_H
#define AXISWIDGET_H

#include "axisslider.h"
#include "axisdisplay.h"

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

class AxisWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AxisWidget(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent = 0, VALUE_TYPE type = VALUE_TYPE::DOUBLE);

    void setZoomFactor(double factor);
    void setAxisMargin(int margin);
    void setTickCount(int ticks);
    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);

    void setMin(double min);
    void setMax(double max);

    void setRange(double min, double max, bool updateDisplay = false);
    void setRange(QPair<double, double> range, bool updateDisplay = false);
    QPair<double, double> getRange();

    void zoom(double factor);
    void pan(double dx, double dy);

    void toggleDisplayFormat(TIME_DISPLAY_FORMAT format);

    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void displayedMinChanged(double min);
    void displayedMaxChanged(double max);

    void minRatio(double ratio);
    void maxRatio(double ratio);

public slots:
    void setDisplayedMin(double min);
    void setDisplayedMax(double max);
    void setDisplayedRange(double min, double max);

    void hoverLineUpdated(bool visible, QPointF pos = QPointF());

private slots:
    void minRatioChanged(double ratio);
    void maxRatioChanged(double ratio);

protected:
    void wheelEvent(QWheelEvent *event);

private:
    AxisSlider* _slider = 0;
    AxisDisplay* _display = 0;

    Qt::Orientation _orientation;
    Qt::Alignment _alignment;

    double _min;
    double _max;
    double _range;
    double _displayedMin = -1;
    double _displayedMax = -1;
    double zoomFactor = 1.1;

};

#endif // AXISWIDGET_H

