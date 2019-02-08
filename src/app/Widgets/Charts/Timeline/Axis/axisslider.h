#ifndef AXISSLIDER_H
#define AXISSLIDER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPainter>

enum class HIT_RECT{NO_SLIDER, MIN_SLIDER, MID_SLIDER, MAX_SLIDER};

class AxisSlider : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NO_DRAG_MODE, PAN_READY_MODE, PAN_MODE};

    explicit AxisSlider(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent = 0);

    Qt::Orientation getOrientation();
    Qt::Alignment getAlignment();
    double getAxisPenWidth();

    double getMinRatio();
    double getMaxRatio();

    void setZoomFactor(double factor);

signals:
    void minRatioChanged(double ratio);
    void maxRatioChanged(double ratio);

public slots:
    void updateMinRatio(double ratio);
    void updateMaxRatio(double ratio);

    void zoom(double factor);
    void pan(double dx, double dy);

protected:
    void themeChanged();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void leaveEvent(QEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    HIT_RECT getCursorHitRect(QPointF cursorPos);

    void moveSliders(double min, double max);
    void moveSliderRects(double min, double max);

    void minSliderMoved(double min);
    void maxSliderMoved(double max);

    void updateSlidersOnSizeChange();

    Qt::Orientation _orientation;
    Qt::Alignment _alignment;

    QColor _axisColor;
    QColor _sliderColor;

    QPen _axisPen;
    QBrush _middleBrush;
    QBrush _highlightBrush;

    QRectF _minSlider;
    QRectF _maxSlider;
    QRectF _midSlider;

    DRAG_MODE dragMode = NO_DRAG_MODE;
    HIT_RECT sliderHitRect = HIT_RECT::NO_SLIDER;
    QPointF sliderOrigin = QPointF(-1, -1);
    QPointF cursorPoint = QPointF(-1, -1);

    HIT_RECT prevClickedRect = HIT_RECT::NO_SLIDER;
    double zoomFactor = 1.1;

    double _minRatio;
    double _maxRatio;
    double _sliderMin;
    double _sliderMax;
    double _sliderRange;
    double _actualMin;
    double _actualMax;

};

#endif // AXISSLIDER_H
