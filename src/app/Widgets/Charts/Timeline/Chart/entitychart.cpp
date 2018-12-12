#include "entitychart.h"
#include "../../../../theme.h"
#include "../../Series/barseries.h"
#include "../../Series/stateseries.h"

#include <QPainter>
#include <QPainter>
#include <algorithm>
#include <iostream>

#define BACKGROUND_OPACITY 0.25
#define LINE_WIDTH 2.0
#define POINT_BORDER 2.0
#define HIGHLIGHT_PEN_WIDTH POINT_BORDER + 1.0

#define BAR_PEN_WIDTH 1.0
#define BAR_WIDTH 5.0
#define PRINT_RENDER_TIMES false

/**
 * @brief EntityChart::EntityChart
 * @param item
 * @param parent
 */
EntityChart::EntityChart(ViewItem* item, QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    _viewItem = item;
    _dataMinX = DBL_MAX;
    _dataMaxX = DBL_MIN;
    _dataMinY = DBL_MAX;
    _dataMaxY = DBL_MIN;

    //_color_s_state = 35;
    _color_s_state = 50;
    _color_s_notification = 100;
    _color_s_line = 200;

    connect(Theme::theme(), &Theme::theme_Changed, this, &EntityChart::themeChanged);
    themeChanged();

    _seriesKindVisible[TIMELINE_SERIES_KIND::LINE] = true;
}


/**
 * @brief EntityChart::getViewItem
 * @return
 */
ViewItem* EntityChart::getViewItem()
{
    return _viewItem;
}


/**
 * @brief EntityChart::getViewItemID
 * @return
 */
int EntityChart::getViewItemID()
{
    if (_viewItem) {
        return _viewItem->getID();
    }
    return -1;
}


/**
 * @brief EntityChart::addEventSeries
 * @param series
 */
void EntityChart::addEventSeries(MEDEA::EventSeries* series)
{
    if (series) {
        _eventSeries = series;
        _seriesList[series->getKind()] = series;
    }
}


/**
 * @brief EntityChart::removeEventSeries
 * @param ID
 */
void EntityChart::removeEventSeries(QString ID)
{
    if (_eventSeries && _eventSeries->getID() == ID) {
        _eventSeries = 0;
        _seriesList.remove(_eventSeries->getKind());
    }
}


/**
 * @brief EntityChart::addSeries
 * @param series
 */
void EntityChart::addSeries(MEDEA::DataSeries* series)
{
    if (!series)
        return;

    _seriesList[series->getKind()] = series;
    _seriesKindVisible[series->getKind()] = true;

    connect(series, &MEDEA::DataSeries::pointsAdded, this, &EntityChart::pointsAdded);
    series->pointsAdded((series->getConstPoints()));

    connect(series, &MEDEA::DataSeries::rangeXChanged, this, &EntityChart::rangeXChanged);
    rangeXChanged(series->getRangeX().first, series->getRangeX().second);

    if (series->isLineSeries() || series->isBarSeries()) {
        _containsYRange = true;
        connect(series, &MEDEA::DataSeries::rangeYChanged, this, &EntityChart::rangeYChanged);
        rangeYChanged(series->getRangeY().first, series->getRangeY().second);
    }
}


/**
 * @brief EntityChart::removeSeries
 * @param seriesKind
 */
void EntityChart::removeSeries(TIMELINE_SERIES_KIND seriesKind)
{
    _seriesList.remove(seriesKind);
    _mappedPoints.remove(seriesKind);
    _containsYRange = !_seriesList.value(TIMELINE_SERIES_KIND::LINE, 0) || !_seriesList.value(TIMELINE_SERIES_KIND::BAR, 0);
    update();
}


/**
 * @brief EntityChart::getSeries
 * @return
 */
const QHash<TIMELINE_SERIES_KIND, MEDEA::EventSeries*>& EntityChart::getSeries()
{
    return _seriesList;
}


/**
 * @brief EntityChart::getHovereSeriesKinds
 * @return
 */
const QList<TIMELINE_SERIES_KIND> EntityChart::getHovereSeriesKinds()
{
    return _hoveredSeriesTimeRange.keys();
}


/**
 * @brief EntityChart::getHoveredTimeRange
 * @param kind
 * @return
 */
const QPair<qint64, qint64> EntityChart::getHoveredTimeRange(TIMELINE_SERIES_KIND kind)
{
    return _hoveredSeriesTimeRange.value(kind, {-1, -1});
}


/**
 * @brief EntityChart::getRangeX
 * @return
 */
QPair<double, double> EntityChart::getRangeX()
{
    return {_dataMinX, _dataMaxX};
}


/**
 * @brief EntityChart::getRangeY
 * @return
 */
QPair<double, double> EntityChart::getRangeY()
{
    return {_dataMinY, _dataMaxY};
}


/**
 * @brief EntityChart::isHovered
 * @return
 */
bool EntityChart::isHovered()
{
    return _hovered;
}


/**
 * @brief EntityChart::setHovered
 * @param visible
 */
void EntityChart::setHovered(bool visible)
{
    _hovered = visible;
}


/**
 * @brief EntityChart::setHoveredRect
 * @param rect
 */
void EntityChart::setHoveredRect(QRectF rect)
{
    if (rect != _hoveredRect) {
        QPoint pos = mapFromParent(rect.topLeft().toPoint());
        rect.moveTo(pos.x(), 0);
        _hoveredRect = rect;
    }
}


/**
 * @brief EntityChart::setSeriesKindVisible
 * @param kind
 * @param visible
 */
void EntityChart::setSeriesKindVisible(TIMELINE_SERIES_KIND kind, bool visible)
{
    _seriesKindVisible[kind] = visible;
    update();
}


/**
 * @brief EntityChart::seriesKindHovered
 * @param kind
 */
void EntityChart::seriesKindHovered(TIMELINE_SERIES_KIND kind)
{
    if (kind == _hoveredSeriesKind)
        return;

    _stateColor = _backgroundColor;
    _notificationColor = _backgroundColor;
    _lineColor = _backgroundColor;
    _utilisationColor = _backgroundColor;

    switch (kind) {
    case TIMELINE_SERIES_KIND::STATE:
        _stateColor = _defaultStateColor;
        break;
    case TIMELINE_SERIES_KIND::NOTIFICATION:
        _notificationColor = _defaultNotificationColor;
        break;
    case TIMELINE_SERIES_KIND::BAR:
    case TIMELINE_SERIES_KIND::LINE:
        _lineColor = _defaultLineColor;
        break;
    case TIMELINE_SERIES_KIND::CPU_UTILISATION:
        _utilisationColor = _defaultUtilisationColor;
        break;
    default: {
        // clear hovered state
        _stateColor = _defaultStateColor;
        _notificationColor = _defaultNotificationColor;
        _lineColor = _defaultLineColor;
        _utilisationColor = _defaultUtilisationColor;
        break;
    }
    }

    _hoveredSeriesKind = kind;
    update();
}


/**
 * @brief EntityChart::themeChanged
 */
void EntityChart::themeChanged()
{
    Theme* theme = Theme::theme();
    bool darkTheme = theme->getTextColor() == theme->white();

    setFont(theme->getSmallFont());

    int min = darkTheme ? 80 : 50;
    int r = min + rand() % 160;
    int g = min + rand() % 160;
    int b = min + rand() % 160;

    QColor color = QColor(r, g, b);
    qreal delta = 150;

    _borderColorDelta = darkTheme ? 150 : 50;
    _colorDelta = darkTheme ? -50 : 50;

    _barColor = color;
    _pointColor = darkTheme ? color.darker(delta) : color.lighter(delta);
    _pointBorderColor = darkTheme ? color.lighter(delta) : color.darker(delta);

    _color_v_notification = darkTheme ? 130 : 200;
    color.setHsv(rand() % 45, _color_s_notification, _color_v_notification);
    _defaultNotificationColor = color;

    _color_v_line = 150; //darkTheme ? 130 : 180;
    color.setHsv(150 + rand() % 60, _color_s_line, _color_v_line);
    _defaultLineColor = color;

    _color_v_state = darkTheme ? 80 : 180;
    color.setHsv(210 + rand() % 60, _color_s_state, _color_v_state);
    _defaultStateColor = color;

    _stateColor = _defaultStateColor;
    _notificationColor = _defaultNotificationColor;
    _lineColor = _defaultLineColor;

    _backgroundColor = theme->getAltBackgroundColor();
    _backgroundColor.setAlphaF(BACKGROUND_OPACITY);
    _highlightColor = theme->getHighlightColor();
    _highlightTextColor = theme->getTextColor(ColorRole::SELECTED);
    _hoveredRectColor = theme->getActiveWidgetBorderColor();

    _gridPen = QPen(theme->getAltTextColor(), 0.5);
    _pointPen = QPen(_pointColor, _pointWidth, Qt::SolidLine, Qt::RoundCap);
    _pointBorderPen = QPen(_pointBorderColor, _pointWidth + POINT_BORDER * 2, Qt::SolidLine, Qt::RoundCap);
    _highlightPointPen = QPen(theme->getTextColor(), POINT_BORDER + 1.5, Qt::SolidLine, Qt::RoundCap);
    _hoverLinePen = QPen(theme->getTextColor(), POINT_BORDER, Qt::PenStyle::DotLine);
    _highlightPen = QPen(theme->getHighlightColor(), HIGHLIGHT_PEN_WIDTH);

    _messagePixmap = theme->getImage("Icons", "exclamation", QSize(), theme->getMenuIconColor());
}


/**
 * @brief EntityChart::rangeXChanged
 * @param min
 * @param max
 */
void EntityChart::rangeXChanged(double min, double max)
{
    bool changed = min < _dataMinX || max > _dataMaxX;
    if (changed) {
        _dataMinX = qMin(min, _dataMinX);
        _dataMaxX = qMax(max, _dataMaxX);
        emit dataRangeXChanged(_dataMinX, _dataMaxX);
    }
}


/**
 * @brief EntityChart::rangeYChanged
 * @param min
 * @param max
 */
void EntityChart::rangeYChanged(double min, double max)
{
    if (min < _dataMinY)
        _dataMinY = min;
    if (max > _dataMaxY)
        _dataMaxY = max;
}


/**
 * @brief EntityChart::pointsAdded
 * @param points
 */
void EntityChart::pointsAdded(QList<QPointF> points)
{
    MEDEA::DataSeries* series = qobject_cast<MEDEA::DataSeries*>(sender());
    if (series) {
        _seriesPoints[series->getKind()].append(points);
        update();
        //emit dataAdded(points);
    }
}


/**
 * @brief EntityChart::resizeEvent
 * @param event
 */
void EntityChart::resizeEvent(QResizeEvent* event)
{
    update();
    QWidget::resizeEvent(event);
}


/**
 * @brief EntityChart::paintEvent
 * @param event
 */
void EntityChart::paintEvent(QPaintEvent* event)
{
    /*
     * NOTE - This assumes that the data is ordered in ascending order time-wise
     */

    auto start = QDateTime::currentMSecsSinceEpoch();
    const auto& paintOrder = GET_TIMELINE_SERIES_KINDS();
    //const static QList<TIMELINE_SERIES_KIND> paintOrder({TIMELINE_SERIES_KIND::STATE, TIMELINE_SERIES_KIND::NOTIFICATION, TIMELINE_SERIES_KIND::LINE, TIMELINE_SERIES_KIND::BAR});

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);

    clearHoveredLists();

    for (const auto& kind : paintOrder) {
        if (kind != _hoveredSeriesKind)
            paintSeries(painter, kind);
    }
    paintSeries(painter, _hoveredSeriesKind);

    QColor penColor = _gridPen.color();
    qreal penWidth = _gridPen.widthF();

    // display the y-range and send the series points that were hovered over
    if (_hovered) {
        if (_containsYRange) {
            QString minStr = QString::number(round(_dataMinY));
            QString maxStr = QString::number(round(_dataMaxY));
            int h = fontMetrics().height();
            int w = qMax(fontMetrics().width(minStr), fontMetrics().width(maxStr)) + 5;
            QRectF maxRect(width() - w, POINT_BORDER, w, h);
            QRectF minRect(width() - w, height() - h - POINT_BORDER, w, h);
            painter.setPen(_hoverLinePen.color());
            painter.setBrush(_hoveredRectColor);
            painter.drawRect(maxRect);
            painter.drawRect(minRect);
            painter.drawText(maxRect, maxStr, QTextOption(Qt::AlignCenter));
            painter.drawText(minRect, minStr, QTextOption(Qt::AlignCenter));
        }
        penColor = _hoverLinePen.color();
        penWidth = 4.0;
    }

    // NOTE: Don't use rect.bottom (rect.bottom = rect.top + rect.height - 1)
    // draw horizontal grid lines
    painter.setPen(QPen(penColor, penWidth));
    painter.drawLine(0, 0, rect().right(), 0);
    painter.drawLine(0, height(), rect().right(), height());

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "Total Series Render Took: " << finish - start << "MS. - " << _viewItem->getData("label").toString();
}


/**
 * @brief EntityChart::paintEventSeries
 * @param painter
 */
void EntityChart::paintEventSeries(QPainter &painter)
{
    if (!_eventSeries)
        return;

    double barWidth = 10.0; //BAR_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<MEDEA::Event*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;
    double current_left = _displayedMin;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = _eventSeries->getEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        if (current_time > _displayedMin) {
            break;
        }
    }

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endTimes.constBegin();
    auto end_bucket_ittr = bucket_endTimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& current_time = (*current)->getTimeMS();
        while (current_bucket_ittr != end_bucket_ittr) {
            if (current_time > (*current_bucket_ittr)) {
                current_bucket_ittr ++;
                current_bucket ++;
            } else {
                break;
            }
        }
        if (current_bucket < barCount) {
            buckets[current_bucket].append(*current);
        }
    }

    auto max = 0.0;
    for (const auto& data : buckets) {
        for (auto e : data) {
            auto event = (CPUUtilisationEvent*) e;
            max = qMax(max, event->getUtilisation());
        }
    }

    auto availableHeight = height() - barWidth;
    QColor seriesColor = _utilisationColor;
    QList<QRectF> rects;

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        auto event = (CPUUtilisationEvent*) buckets[i][0];
        auto utilisation = event->getUtilisation();
        double y = (1 - utilisation / max) * availableHeight;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        rects.append(rect);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);

    for (int i = 0; i < rects.count() - 1; i++) {
        auto rect1 = rects.at(i);
        auto rect2 = rects.at(i + 1);
        painter.setPen(QPen(seriesColor, 3.0));
        painter.drawLine(rect1.center(), rect2.center());
        painter.setPen(QPen(seriesColor, 2));
        if (rectHovered(_eventSeries->getKind(), rect1)) {
            painter.setPen(QPen(_highlightTextColor, 2.0));
            painter.setBrush(_highlightColor);
        } else {
            painter.setBrush(seriesColor);
        }
        painter.drawEllipse(rect1);
        painter.setPen(QPen(seriesColor, 2));
        if (rectHovered(_eventSeries->getKind(), rect2)) {
            painter.setPen(QPen(_highlightTextColor, 2.0));
            painter.setBrush(_highlightColor);
        } else {
            painter.setBrush(seriesColor);
        }
        painter.drawEllipse(rect2);
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
}


/**
 * @brief EntityChart::paintSeries
 * @param painter
 * @param kind
 */
void EntityChart::paintSeries(QPainter &painter, TIMELINE_SERIES_KIND kind)
{
    if (!_seriesKindVisible.value(kind, false))
        return;

    // draw the points and get intersection point(s) index
    switch (kind) {
    case TIMELINE_SERIES_KIND::STATE:
        paintStateSeries(painter);
        break;
    case TIMELINE_SERIES_KIND::NOTIFICATION:
        paintNotificationSeries(painter);
        break;
    case TIMELINE_SERIES_KIND::BAR:
        paintBarSeries(painter);
        break;
    case TIMELINE_SERIES_KIND::CPU_UTILISATION:
        paintEventSeries(painter);
    default:
        //qWarning("EntityChart::paintSeries - Series kind not handled");
        break;
    }
}


/**
 * @brief EntityChart::paintNotificationSeries
 * @param painter
 */
void EntityChart::paintNotificationSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    MEDEA::DataSeries* series = (MEDEA::DataSeries*)_seriesList.value(TIMELINE_SERIES_KIND::NOTIFICATION, 0);
    if (!series)
        return;

    // TODO - Calculate lower and upper bounds once in the main paint method???
    const auto& points = series->getConstPoints();
    auto current = std::lower_bound(points.cbegin(), points.cend(), _displayedMin, [](const QPointF &p, const double &value) {
        return p.x() < value;
    });
    auto upper = std::upper_bound(points.cbegin(), points.cend(), _displayedMax, [](const double &value, const QPointF &p) {
        return value < p.x();
    });

    // bucket count
    double barWidth = BAR_WIDTH * 1.5;
    double barCount = ceil((double)width() / barWidth);
    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    QVector< QList<QPointF> > buckets(barCount);
    QList<qint64> bucketEndtimes;

    auto currentTime = _displayedMin;
    for (int i = 0; i < barCount; i++) {
        bucketEndtimes.append(currentTime + barTimeWidth);
        currentTime = bucketEndtimes.last();
    }

    auto currentBucket = 0;
    auto currentBucketIttr = bucketEndtimes.constBegin();
    auto endBucketIttr = bucketEndtimes.constEnd();

    // put the data in the correct bucket
    for (; current != upper; current++) {
        auto current_point = (*current);
        while (currentBucketIttr != endBucketIttr) {
            if (current_point.x() > (*currentBucketIttr)) {
                currentBucketIttr++;
                currentBucket++;
            } else {
                break;
            }
        }
        if (currentBucket < barCount) {
            buckets[currentBucket].append(current_point);
        }
    }

    auto max = 0;
    for (const auto& data : buckets) {
        max = qMax(max, data.size());
    }

    QColor seriesColor = _notificationColor;
    painter.setPen(QPen(seriesColor.lighter(_borderColorDelta), BAR_PEN_WIDTH));

    double dataToPixel = (double) height() / max;
    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].size();
        QRectF rect = QRect(i * barWidth, height() - count * dataToPixel, barWidth, count * dataToPixel);
        auto color = count == 1 ? seriesColor : seriesColor.darker(100 * (1 + count / (double)max));
        if (rectHovered(TIMELINE_SERIES_KIND::NOTIFICATION, rect)) {
            color.setHsv(qAbs(color.hue() - 180), 255, 255);
        }
        painter.setBrush(color);
        painter.drawRect(rect);
    }

    /*
    QPen defaultPen(_pointBorderColor, 1.5);
    painter.setBrush(_pointColor);

    for (QPointF p : points) {
        QRect rect(p.x() - size / 2, p.y() - size / 2, size, size);
        painter.fillRect(rect, _pointBorderColor);
        painter.setPen(pointHovered(TIMELINE_SERIES_KIND::NOTIFICATION, p, points.indexOf(p)) ?  _highlightPen : defaultPen);
        painter.drawRoundedRect(rect.adjusted(1, -2, -1, 2), 4, 4);
        painter.drawPixmap(rect, _messagePixmap);
    }
    */

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "NotificationSeries Render Took: " << finish - start << "MS. Returned: " << points.count() ;
}


/**
 * @brief EntityChart::paintStateSeries
 * @param painter
 */
void EntityChart::paintStateSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    MEDEA::StateSeries* series = (MEDEA::StateSeries*)_seriesList.value(TIMELINE_SERIES_KIND::STATE, 0);
    if (!series)
        return;

    // bucket count
    double barWidth = BAR_WIDTH * 2.0;
    double barCount = ceil((double)width() / barWidth);
    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    // each bucket contains the number of events still running at that time
    QVector<int> buckets(barCount);
    QList<qint64> bucketEndTimes;

    auto currentTime = _displayedMin;
    for (int i = 0; i < barCount; i++) {
        bucketEndTimes.append(currentTime + barTimeWidth);
        currentTime = bucketEndTimes.last();
    }

    auto bucketIndex = 0;
    auto currentBucket = bucketEndTimes.constBegin();
    auto endBucket = bucketEndTimes.constEnd();

    const auto& lines = series->getLines();
    QList<QRectF> lineRects;
    for (auto& line : lines) {
        if (line.x1() > _displayedMax)
            continue;
        if (line.x2() < _displayedMin)
            continue;
        lineRects.append(QRectF(QPointF(line.x1(), 0), QPointF(line.x2(), LINE_WIDTH)));
    }

    auto prevX = _displayedMin;
    auto minCount = 100;
    auto maxCount = 0;

    // increment the correct bucket
    for (; currentBucket != endBucket; currentBucket++) {
        auto currentX = *currentBucket;
        auto currentLineRect = lineRects.begin();
        QRectF currentBucketRect(QPointF(prevX, 0), QPointF(currentX, height()));
        while (currentLineRect != lineRects.end()) {
            if ((*currentLineRect).right() < currentBucketRect.left()) {
                currentLineRect = lineRects.erase(currentLineRect);
                if (currentLineRect == lineRects.end())
                    break;
            }
            if (currentBucketRect.intersects(*currentLineRect)) {
                buckets[bucketIndex]++;
            }
            currentLineRect++;
        }
        minCount = qMin(buckets[bucketIndex], minCount);
        maxCount = qMax(buckets[bucketIndex], maxCount);
        prevX = currentX;
        bucketIndex++;
    }

    // draw the buckets from the bottom using a painter path
    QColor seriesColor = _stateColor;

    /*
     * TRYING GRADIENTS
     *
    QLinearGradient gradient;
    QRectF gradientRect, prevRect;
    QColor prevColor;

    painter.setPen(Qt::NoPen);

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i];
        if (count == 0) {
            if (!prevRect.isNull()) {
                painter.fillRect(prevRect.center().x(), prevRect.y(), prevRect.width() / 2.0, height(), prevColor);
                prevRect = QRect();
            }
            continue;
        }

        QRectF rect(i * barWidth, 0, barWidth, height());
        QColor color = count == 1 ? seriesColor : seriesColor.darker(100 + (25 * (count - 1)));
        if (prevRect.isNull()) {
            painter.fillRect(rect.x(), rect.y(), barWidth / 2.0, height(), color);
            prevRect = rect;
            prevColor = color;
            continue;
        }

        qreal centerY = height() / 2.0;
        gradientRect.setRect(prevRect.center().x(), 0, barWidth, height());
        gradient.setStart(gradientRect.x(), centerY);
        gradient.setFinalStop(gradientRect.right(), centerY);
        gradient.setColorAt(0, prevColor);
        gradient.setColorAt(1, color);
        painter.fillRect(gradientRect, gradient);

        prevRect = rect;
        prevColor = color;
    }
    */

    /*
     * SAME PAINT METHOD AS BELOW WITHOUT THE RECT HEIGHT
     */
    for (int i = 0; i < barCount; i++) {
        int count = buckets[i];
        double y = height() - ((double) (height() - 0.5) * count / maxCount);
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, 0, barWidth, height());
        QColor color = count == 1 ? seriesColor : seriesColor.darker(100 + (50 * (count - 1)));
        if (rectHovered(TIMELINE_SERIES_KIND::STATE, rect)) {
            color.setHsv(qAbs(color.hue() - 180), 255, _color_v_state + 75);
        }
        painter.fillRect(rect, color);
    }/*/

    /*
     * THIS FILLS THE SERIES RECTS THEN PAINTS THE OUTLINE OF THE OVERALL PATH
     *
    QPainterPath path;
    for (int i = 0; i < barCount; i++) {
        int count = buckets[i];
        double y = height() - ((double) (height() - 0.5) * count / maxCount);
        QRectF rect(i * barWidth, y, barWidth, height() - y);
        if (i == 0) {
            path.moveTo(rect.topLeft());
        } else {
            path.lineTo(rect.topLeft());
        }
        path.lineTo(rect.topRight());

        int colorIncr = 50;
        QColor color = count == 1 ? seriesColor : seriesColor.darker(100 + (colorIncr * (count - 1)));
        if (pointHovered(TIMELINE_SERIES_KIND::BAR, rect, 10)) {
            color.setHsv(qAbs(color.hue() - 180), 255, _color_v_state + 75);
        }

        painter.fillRect(rect, color);
    }

    // this draws the outline of the path; not including the outlines of the bar rects
    painter.strokePath(path, QPen(_gridPen.color(), 1));
    */

    /*
     * ORIGINAL PAINT METHOD FOR INDIVIDUAL EVENTS
     *
    QPen pen(_gridPen.color(), POINT_BORDER);
    double halfPenWidth = POINT_BORDER / 2.0;
    int startAngle = 90 * 16;
    int endAngle = 270 * 16;
    int spanAngle = 180 * 16;

    for (int i = 1; i < points.size(); i += 2) {
        QPointF p1 = points.at(i - 1);
        QPointF p2 = points.at(i);
        QRectF rect1(p1.x() - size * 0.25, p1.y() - size / 2, size, size);
        QRectF rect2(p2.x() - size * 0.75, p2.y() - size / 2, size, size);
        painter.setPen(QPen(_gridPen.color(), LINE_WIDTH));
        painter.drawLine(p1, p2);
        painter.setBrush(Qt::white);
        painter.setPen(pen);
        if (pointHovered(TIMELINE_SERIES_KIND::STATE, p1, i - 1)) {
            rect1.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);
            painter.setPen(_highlightPen);
        }
        painter.drawChord(rect1, startAngle, spanAngle);
        painter.setBrush(Qt::black);
        painter.setPen(pen);
        if (pointHovered(TIMELINE_SERIES_KIND::STATE, p2, i)) {
            rect2.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);
            painter.setPen(_highlightPen);
        }
        painter.drawChord(rect2, endAngle, spanAngle);
    }
    */

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "StateSeries Render Took: " << finish - start << "MS. Returned: " << lines.count() * 2;
}


/**
 * @brief EntityChart::paintBarSeries
 * @param painter
 * @param points
 */
void EntityChart::paintBarSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    MEDEA::BarSeries* series = (MEDEA::BarSeries*)_seriesList.value(TIMELINE_SERIES_KIND::BAR, 0);
    if (!series)
        return;

    if (!_seriesKindVisible.value(TIMELINE_SERIES_KIND::LINE, false))
        return;

    const auto& data = series->getConstData2();
    auto current = data.lowerBound(_displayedMin);
    auto upper = data.upperBound(_displayedMax);

    double barWidth = BAR_WIDTH;
    int barCount = ceil(width() / barWidth);

    // because barCount needed to be rounded up, the barWidth also needs to be recalculated
    barWidth = (double) width() / barCount;

    // bar/bucket count
    QVector< QList<MEDEA::BarData*> > buckets(barCount);
    QList<double> bucketEndtimes;
    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;
    
    auto currentLeft = _displayedMin;
    for (int i = 0; i < barCount; i++) {
        bucketEndtimes.append(currentLeft + barTimeWidth);
        currentLeft = bucketEndtimes.last();
    }

    auto currentBucket = 0;
    auto currentBucketIttr = bucketEndtimes.constBegin();
    auto endBucketIttr = bucketEndtimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& currentX = current.key();
        while (currentBucketIttr != endBucketIttr) {
            if (currentX > (*currentBucketIttr)) {
                currentBucketIttr++;
                currentBucket++;
            } else {
                break;
            }
        }
        if (currentBucket < barCount)
            buckets[currentBucket] += current.value();
    }

    QColor seriesColor = _lineColor;
    painter.setPen(QPen(seriesColor.lighter(_borderColorDelta), BAR_PEN_WIDTH));

    for (int i = 0; i < barCount; i++) {
        auto size = buckets[i].size();
        if (size > 0) {
            auto dataMin = DBL_MAX;
            auto dataMax = 0.0;
            for (const auto& data : buckets[i]) {
                dataMin = qMin(dataMin, data->getMin());
                dataMax = qMax(dataMax, data->getMax());
            }
            auto dataToPixel = height() / _dataMaxY;
            auto pixelMin = dataMin * dataToPixel;
            auto pixelMax = dataMax * dataToPixel;
            auto barHeight = pixelMax - pixelMin;
            auto color = seriesColor.darker(100 * (1.0 + size / dataMax));
            auto rect = QRectF(i * barWidth, height() - pixelMax, barWidth, barHeight);

            if (rectHovered(TIMELINE_SERIES_KIND::BAR, rect)) {
                color.setHsv(qAbs(color.hue() - 180), 255, 255);
            }
            painter.setBrush(color);

            if (size == 1) {
                const auto& data = buckets[i][0];
                paintBarData(painter, rect, color, data->getData());
                painter.setBrush(Qt::NoBrush);
            }
            painter.drawRect(rect);
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "BarSeries Render Took: " << finish - start << "MS. Returned: " << data.count();
}


/**
 * @brief EntityChart::paintBarData
 * @param painter
 * @param barRect
 * @param color
 * @param data
 */
void EntityChart::paintBarData(QPainter &painter, const QRectF &barRect, const QColor &color, const QVector<double> &data)
{
    if (data.isEmpty() || data.count() > 5)
        return;

    double dataToPixel = height() / _dataMaxY;
    painter.fillRect(barRect, color);

    switch (data.count()) {
    case 5: {
        auto barHeight = (data[1] - data[2]) * dataToPixel;
        auto upperRect = QRectF(barRect.x(), height() - data[1] * dataToPixel, BAR_WIDTH, barHeight);
        barHeight = (data[2] - data[3]) * dataToPixel;
        auto lowerRect = QRectF(barRect.x(), height() - data[2] * dataToPixel, BAR_WIDTH, barHeight);
        painter.fillRect(upperRect, color.lighter());
        painter.fillRect(lowerRect, color.darker());
    }
    case 3: {
        auto midRect = QRectF(barRect.x(), height() - data[1] * dataToPixel - 1, BAR_WIDTH, 2);
        painter.fillRect(midRect, color);
    }
    default:
        break;
    }
}


/**
 * @brief EntityChart::rectHovered
 * @param kind
 * @param hitRect
 * @return
 */
bool EntityChart::rectHovered(TIMELINE_SERIES_KIND kind, const QRectF& hitRect)
{
    auto painterRect = hitRect.adjusted(-BAR_PEN_WIDTH / 2.0, 0, BAR_PEN_WIDTH / 2.0, 0);
    if (rectHovered(painterRect)) {
        // if the hit rect is hovered, store/update the hovered time range for the provided series kind
        QPair<qint64, qint64> timeRange = {mapPixelToTime(hitRect.left()), mapPixelToTime(hitRect.right())};
        if (_hoveredSeriesTimeRange.contains(kind)) {
            timeRange.first = _hoveredSeriesTimeRange.value(kind).first;
        }
        _hoveredSeriesTimeRange[kind] = timeRange;
        return true;
    }
    return false;
}


/**
 * @brief EntityChart::rectHovered
 * @param hitRect
 * @return
 */
bool EntityChart::rectHovered(const QRectF &hitRect)
{
    return _hoveredRect.intersects(hitRect);
}


/**
 * @brief EntityChart::clearHoveredLists
 */
void EntityChart::clearHoveredLists()
{
    // clear previously hovered series kinds
    _hoveredSeriesTimeRange.clear();
}


/**
 * @brief EntityChart::setPointWidth
 * @param width
 */
void EntityChart::setPointWidth(double width)
{
    _pointWidth = width;
    _pointPen.setWidthF(width);
    _pointBorderPen.setWidthF(width + POINT_BORDER * 2);
    update();
}


/**
 * @brief EntityChart::getPointWidth
 * @param kind
 * @return
 */
double EntityChart::getPointWidth(TIMELINE_SERIES_KIND kind)
{
    switch (kind) {
    case TIMELINE_SERIES_KIND::NOTIFICATION:
        //return _pointWidth * 1.15;
        return _pointWidth;
    case TIMELINE_SERIES_KIND::STATE:
        return _pointWidth * 1.15 + POINT_BORDER;
    case TIMELINE_SERIES_KIND::LINE:
        return _pointWidth / 1.5;
    case TIMELINE_SERIES_KIND::BAR:
        return BAR_WIDTH;
    default:
        return _pointWidth;
    }
}


/**
 * @brief EntityChart::setMin
 * @param min
 */
void EntityChart::setMin(double min)
{
    _displayedMin = min;
    update();
}


/**
 * @brief EntityChart::setMax
 * @param max
 */
void EntityChart::setMax(double max)
{
    _displayedMax = max;
    update();
}


/**
 * @brief EntityChart::setRange
 * @param min
 * @param max
 */
void EntityChart::setRange(double min, double max)
{
    _displayedMin = min;
    _displayedMax = max;
    update();
}


/**
 * @brief EntityChart::mapPixelToTime
 * @param x
 * @return
 */
qint64 EntityChart::mapPixelToTime(double x)
{
    auto timeRange = _displayedMax - _displayedMin;
    auto ratio = x / width();
    //time = _displayedMin + timeRange * ratio;
    return _displayedMin + timeRange * ratio;
}


/**
 * @brief EntityChart::mapTimeToPixel
 * @param time
 * @return
 */
double EntityChart::mapTimeToPixel(double time)
{
    //(time - _displayedMin) / (_displayedMax - _displayedMin) * width() = x;
    auto timeRange = _displayedMax - _displayedMin;
    auto adjustedTime = time - _displayedMin;
    return adjustedTime / timeRange * width();

}


/**
 * @brief qHash
 * @param key
 * @param seed
 * @return
 */
inline uint qHash(TIMELINE_SERIES_KIND key, uint seed)
{
    return ::qHash(static_cast<uint>(key), seed);
}
