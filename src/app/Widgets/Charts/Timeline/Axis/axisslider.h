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

    explicit AxisSlider(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent = nullptr);

    Qt::Orientation getOrientation() const;
    Qt::Alignment getAlignment() const;
    double getAxisPenWidth() const;

    double getMinRatio() const;
    double getMaxRatio() const;

    void setZoomFactor(double factor);
    void setPanning(bool panning);

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

    Qt::Orientation orientation_;
    Qt::Alignment alignment_;

    QColor axisColor_;
    QColor sliderColor_;

    QPen axisPen_;
    QBrush middleBrush_;
    QBrush highlightBrush_;

    QRectF minSlider_;
    QRectF maxSlider_;
    QRectF midSlider_;

    DRAG_MODE dragMode_ = NO_DRAG_MODE;
    HIT_RECT sliderHitRect_ = HIT_RECT::NO_SLIDER;
    QPointF sliderOrigin_ = QPointF(-1, -1);
    QPointF cursorPoint_ = QPointF(-1, -1);

    HIT_RECT prevClickedRect_ = HIT_RECT::NO_SLIDER;
    double zoomFactor_ = 1.1;

    double minRatio_;
    double maxRatio_;
    double sliderMin_;
    double sliderMax_;
    double sliderRange_;
    double actualMin_;
    double actualMax_;

    bool isPanning_ = false;

};

#endif // AXISSLIDER_H
