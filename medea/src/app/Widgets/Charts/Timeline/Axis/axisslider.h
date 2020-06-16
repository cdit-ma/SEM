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

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

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

    // TODO: Consider replacing with getter functions to maintain invariants (and also store less)
    double minRatio_ = 0;
    double maxRatio_ = 1;
    double sliderMin_ = 0;
    double sliderMax_ = 1;
    double sliderRange_ = 1;
    double actualMin_ = 0;
    double actualMax_ = 1;

    bool isPanning_ = false;
};

#endif // AXISSLIDER_H
