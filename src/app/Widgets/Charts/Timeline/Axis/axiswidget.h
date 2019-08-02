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
    explicit AxisWidget(Qt::Orientation orientation, Qt::Alignment alignment, VALUE_TYPE type = VALUE_TYPE::DOUBLE, QWidget* parent = 0);

    void setZoomFactor(double factor);
    void setAxisMargin(int margin);
    void setTickCount(int ticks);
    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);

    void setPanning(bool panning);

    void setDisplayFormat(TIME_DISPLAY_FORMAT format);

    void setRange(QPair<double, double> range, bool updateDisplay = false);
    void setRange(double min, double max, bool updateDisplay = false);
    void setDisplayRange(double min, double max);

    QPair<double, double> getRange();
    QPair<double, double> getDisplayedRange();

    void zoom(double factor);
    void pan(double dx, double dy);

    bool eventFilter(QObject* watched, QEvent* event);

signals:
    void minRatioChanged(double ratio);
    void maxRatioChanged(double ratio);

public slots:
    void hoverLineUpdated(bool visible, QPointF pos = QPointF());

protected:
    void wheelEvent(QWheelEvent* event);

private:
    void updateMinSliderRatio(double min);
    void updateMaxSliderRatio(double max);

    Qt::Orientation orientation_;
    Qt::Alignment alignment_;

    AxisSlider slider_;
    AxisDisplay display_;

    double min_;
    double max_;
    double range_;
    double zoomFactor_ = 1.1;

};

#endif // AXISWIDGET_H

