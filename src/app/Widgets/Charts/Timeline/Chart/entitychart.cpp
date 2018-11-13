#include "entitychart.h"
#include "../../../../theme.h"
#include "../../Series/barseries.h"
#include "../../Series/stateseries.h"

#include <QPainter>
#include <QPainter>
#include <algorithm>

#define BACKGROUND_OPACITY 0.25
#define LINE_WIDTH 2.0
#define POINT_BORDER 2.0
#define HIGHLIGHT_PEN_WIDTH POINT_BORDER + 1.0

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


void EntityChart::addLifeCycleSeries(PortLifecycleEventSeries* series)
{
    if (!series || series == _lifeCycleSeries)
        return;

    //removeLifeCycleSeries(_lifeCycleSeries->getID());
    _lifeCycleSeries = series;
}


void EntityChart::removeLifeCycleSeries(QString path)
{
    if (_lifeCycleSeries && _lifeCycleSeries->getPortPath() == path) {
        _lifeCycleSeries = 0;
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

    _seriesList[series->getSeriesKind()] = series;
    _seriesKindVisible[series->getSeriesKind()] = true;

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
    getSeriesHitRects(seriesKind).clear();
    update();
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
 * @brief EntityChart::getSeriesColor
 * @return
 */
QColor EntityChart::getSeriesColor()
{
    return _pointColor;
}


/**
 * @brief EntityChart::getSeriesPoints
 * @param seriesKind
 * @return
 */
QList<QPointF> EntityChart::getSeriesPoints(TIMELINE_SERIES_KIND seriesKind)
{
    if (seriesKind == TIMELINE_SERIES_KIND::DATA) {
        QList<QPointF> seriesPoints;
        for (QList<QPointF> points : _seriesPoints.values()) {
            seriesPoints.append(points);
        }
        return seriesPoints;
    } else {
        return _seriesPoints.value(seriesKind);
    }
}


/**
 * @brief EntityChart::resizeEvent
 * @param event
 */
void EntityChart::resizeEvent(QResizeEvent* event)
{
    mapPointsFromRange();
    QWidget::resizeEvent(event);
}


/**
 * @brief EntityChart::paintEvent
 * @param event
 */
void EntityChart::paintEvent(QPaintEvent* event)
{
    const static QList<TIMELINE_SERIES_KIND> paintOrder({TIMELINE_SERIES_KIND::STATE, TIMELINE_SERIES_KIND::NOTIFICATION, TIMELINE_SERIES_KIND::LINE, TIMELINE_SERIES_KIND::BAR});

    auto start = QDateTime::currentMSecsSinceEpoch();

    QPainter painter(this);
    painter.setClipRegion(visibleRegion());
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, false);


    for(const auto& kind : paintOrder){
            if (kind != _hoveredSeriesKind) {
            paintSeries(painter, kind);
        }
    }
    paintSeries(painter, _hoveredSeriesKind);
    paintLifeCycleSeries(painter);

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
        penWidth *= 4.0;
        emit dataHovered(_hoveredPoints);
    }

    // draw horizontal grid lines
    painter.setPen(QPen(_gridPen.color(), penWidth));
    painter.drawLine(rect().topLeft(), rect().topRight());
    painter.drawLine(rect().bottomLeft(), rect().bottomRight());

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "Total Series Render Took: " << finish - start << "MS. - " << _viewItem->getData("label").toString();
}


/**
 * @brief EntityChart::seriesHovered
 * @param kind
 */
void EntityChart::seriesHovered(TIMELINE_SERIES_KIND kind)
{
    if (kind == _hoveredSeriesKind)
        return;

    _stateColor = _backgroundColor;
    _notificationColor = _backgroundColor;
    _lineColor = _backgroundColor;

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
    default: {
        // clear hovered state
        _stateColor = _defaultStateColor;
        _notificationColor = _defaultNotificationColor;
        _lineColor = _defaultLineColor;
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

    _lifeCycleTypePixmaps.insert(LifecycleType::NO_TYPE, theme->getImage("Icons", "circleQuestion", QSize(), theme->getSeverityColor(Notification::Severity::WARNING)));
    _lifeCycleTypePixmaps.insert(LifecycleType::CONFIGURE, theme->getImage("Icons", "gear", QSize(), theme->getMenuIconColor()));
    _lifeCycleTypePixmaps.insert(LifecycleType::ACTIVATE, theme->getImage("Icons", "clockDark", QSize(), theme->getSeverityColor(Notification::Severity::SUCCESS)));
    _lifeCycleTypePixmaps.insert(LifecycleType::PASSIVATE, theme->getImage("Icons", "circleMinusDark", QSize(), theme->getSeverityColor(Notification::Severity::ERROR)));
    _lifeCycleTypePixmaps.insert(LifecycleType::TERMINATE, theme->getImage("Icons", "circleRadio", QSize(), theme->getMenuIconColor()));

    /*
    _lifeCycleTypePixmaps.insert(LifeCycleType::NO_TYPE, theme->getImage("Icons", "circleQuestion")); //, QSize(), theme->getMenuIconColor()));
    _lifeCycleTypePixmaps.insert(LifeCycleType::CONFIGURE, theme->getImage("Icons", "circleInfo", QSize(), theme->getMenuIconColor()));
    _lifeCycleTypePixmaps.insert(LifeCycleType::ACTIVATE, theme->getImage("Icons", "clock", QSize(), theme->getMenuIconColor()));
    _lifeCycleTypePixmaps.insert(LifeCycleType::PASSIVATE, theme->getImage("Icons", "circleMinus", QSize(), theme->getMenuIconColor()));
    _lifeCycleTypePixmaps.insert(LifeCycleType::TERMINATE, theme->getImage("Icons", "circleRadio", QSize(), theme->getMenuIconColor()));
    */
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
        _seriesPoints[series->getSeriesKind()].append(points);
        mapPointsFromRange(series, points);
        //emit dataAdded(points);
    }
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

    // clear previous hovered points
    _hoveredPoints[kind].clear();

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
    default: {
        /*painter.setPen(_pointBorderPen);
        painter.drawPoints(mappedPoints.toVector());
        painter.setPen(_pointPen);
        painter.drawPoints(mappedPoints.toVector());*/
        break;
    }
    }
}


/**
 * @brief EntityChart::paintLifeCycleSeries
 * @param painter
 */
void EntityChart::paintLifeCycleSeries(QPainter &painter)
{
    if (!_lifeCycleSeries)
        return;

    double barWidth = 22.0; //BAR_WIDTH;
    double barCount = ceil((double)width() / barWidth);

    // NOTE - barTimeWidth and bucket_endTimes have to be a double
    // Otherwise if the time range is too small, barTimeWidth is rounded down to 0
    // and bucket_endTimes will consist of the same times, and hence no bar will be drawn

    //qDebug() << "SERIES - " << _lifeCycleSeries->getPortPath();

    QVector< QList<PortLifecycleEvent*> > buckets(barCount);
    QVector<double> bucket_endTimes;
    bucket_endTimes.reserve(barCount);

    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;
    double current_left = _displayedMin;
    for (int i = 0; i < barCount; i++) {
        bucket_endTimes.append(current_left + barTimeWidth);
        current_left = bucket_endTimes.last();
    }

    const auto& events = _lifeCycleSeries->getConstPortEvents();
    auto current = events.constBegin();
    auto upper = events.constEnd();
    for (; current != upper; current++) {
        const auto& current_time = (*current)->getTime();
        if (current_time > _displayedMin) {
            break;
        }
    }

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endTimes.constBegin();
    auto end_bucket_ittr = bucket_endTimes.constEnd();

    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& current_time = (*current)->getTime();
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

    QColor seriesColor = Qt::gray; // Qt::magenta;
    int y = rect().center().y() - barWidth / 2.0;

    for (int i = 0; i < barCount; i++) {
        int count = buckets[i].count();
        if (count == 0)
            continue;
        QRectF rect(i * barWidth, y, barWidth, barWidth);
        if (count == 1) {
            if (pointHovered(rect))
                painter.fillRect(rect, _highlightColor);
            painter.drawPixmap(rect.toRect(), _lifeCycleTypePixmaps.value(buckets[i][0]->getType()));
        } else {
            QColor color = seriesColor.darker(100 + (50 * (count - 1)));
            painter.setPen(Qt::lightGray);
            if (pointHovered(rect)) {
                painter.setPen(_highlightTextColor);
                color = _highlightColor;
            }
            painter.fillRect(rect, color);
            painter.drawText(rect, QString::number(count), QTextOption(Qt::AlignCenter));
        }
    }
}


/**
 * @brief EntityChart::paintNotificationSeries
 * @param painter
 */
void EntityChart::paintNotificationSeries(QPainter &painter)
{
    auto start = QDateTime::currentMSecsSinceEpoch();

    MEDEA::DataSeries* series = _seriesList.value(TIMELINE_SERIES_KIND::NOTIFICATION, 0);
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
    double bar_width = BAR_WIDTH * 1.5; //size;
    double bar_count = ceil((double)width() / bar_width);
    double bar_time_width = (_displayedMax - _displayedMin) / bar_count;

    QVector< QList<QPointF> > buckets(bar_count);
    QList<qint64> bucket_endtimes;

    auto current_time = _displayedMin;
    for (int i = 0; i < bar_count; i++) {
        bucket_endtimes.append(current_time + bar_time_width);
        current_time = bucket_endtimes.last();
    }

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endtimes.constBegin();
    auto end_bucket_ittr = bucket_endtimes.constEnd();

    // put the data in the correct bucket
    for (; current != upper; current++) {
        auto current_point = (*current);
        while (current_bucket_ittr != end_bucket_ittr) {
            if (current_point.x() > (*current_bucket_ittr)) {
                current_bucket_ittr++;
                current_bucket++;
            } else {
                break;
            }
        }
        if (current_bucket < bar_count) {
            buckets[current_bucket].append(current_point);
        }
    }

    auto max = 0;
    for (const auto& data : buckets) {
        max = qMax(max, data.size());
    }

    double dataToPixel = (double) height() / max;

    QColor seriesColor = _notificationColor;
    painter.setPen(QPen(seriesColor.lighter(_borderColorDelta), 1));

    for (int i = 0; i < bar_count; i++) {

        int count = buckets[i].size();

        QRectF rect = QRect(i * bar_width, height() - count * dataToPixel, bar_width, count * dataToPixel);
        auto color = count == 1 ? seriesColor : seriesColor.darker(100 * (1 + count / (double)max));
        if (pointHovered(rect)) {
            //color = _highlightColor;
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
    double barWidth = BAR_WIDTH * 2.0; //getPointWidth(series->getSeriesKind());
    double barCount = ceil((double)width() / barWidth);
    double barTimeWidth = (_displayedMax - _displayedMin) / barCount;

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
        if (pointHovered(rect)) {
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
#include <iostream>
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
    
    const int bar_width = BAR_WIDTH;
    double bar_count = ceil(width() / (double)bar_width);
    double bar_time_width = (_displayedMax - _displayedMin) / bar_count;

    // bar/bucket count
    QVector< QList<MEDEA::BarData*> > buckets(bar_count);
    QList<qint64> bucket_endtimes;
    
    auto current_left = _displayedMin;
    for (int i = 0; i < bar_count; i++) {
        bucket_endtimes.append(current_left + bar_time_width);
        current_left = bucket_endtimes.last();
    }

    auto current_bucket = 0;
    auto current_bucket_ittr = bucket_endtimes.constBegin();
    auto end_bucket_ittr = bucket_endtimes.constEnd();
    
    // put the data in the correct bucket
    for (;current != upper; current++) {
        const auto& current_x = current.key();
        while (current_bucket_ittr != end_bucket_ittr) {
            if (current_x > (*current_bucket_ittr)) {
                current_bucket_ittr ++;
                current_bucket ++;
            } else {
                break;
            }
        }
        if (current_bucket < bar_count) {
            buckets[current_bucket] += current.value();
        }
    }

    auto min_size = INT_MAX;
    auto max_size = 0;
    for (int i = 0; i < bar_count; i ++) {
        auto size = buckets[i].size();
        min_size = qMin(min_size, size);
        max_size = qMax(max_size, size);
    }

    QColor seriesColor = _lineColor;
    painter.setPen(QPen(seriesColor.lighter(_borderColorDelta), 1));

    for (int i = 0; i < bar_count; i ++) {

        auto size = buckets[i].size();
        switch(size) {
        case 0:
            break;
        case 1: {
            const auto& data = buckets[i][0];
            paintBar(painter, data->getData(), i * bar_width, seriesColor);
            break;
        }
        default: {
            auto min = DBL_MAX;
            auto max = 0.0;
            for (const auto& data : buckets[i]) {
                min = qMin(min, data->getMin());
                max = qMax(max, data->getMax());
            }

            //paintBar(painter, {(double)max, (double)min}, i * bar_width, color);

            auto y2pixels = height() / _dataMaxY;
            auto pixel_min = min * y2pixels;
            auto size_pixels = (max-min) * y2pixels;

            auto color = seriesColor.darker(100 * (1 + size / (double)max));
            QRectF rect(i*bar_width, height() - size_pixels - pixel_min, bar_width, size_pixels);
            if (pointHovered(rect)) {
                //color = _highlightColor;
                color.setHsv(qAbs(color.hue() - 180), 255, 255);
            }

            painter.setBrush(color);
            painter.drawRect(rect);
            break;
        }
        }
    }

    auto finish = QDateTime::currentMSecsSinceEpoch();
    if (PRINT_RENDER_TIMES)
        qDebug() << "BarSeries Render Took: " << finish - start << "MS. Returned: " << data.count();
}


/**
 * @brief EntityChart::paintBar
 * @param painter
 * @param data
 * @param x
 */
void EntityChart::paintBar(QPainter &painter, const QVector<double> &data, int x, QColor color)
{
    if (data.isEmpty() || data.count() > 5)
        return;

    QRect barRect, upperRect, lowerRect, midRect;
    double dataToPixel = height() / _dataMaxY;

    switch (data.count()) {
    case 5: {
        auto barHeight = (data[1] - data[2]) * dataToPixel;
        upperRect = QRect(x, height() - data[1] * dataToPixel, BAR_WIDTH, barHeight);
        barHeight = (data[2] - data[3]) * dataToPixel;
        lowerRect = QRect(x, height() - data[2] * dataToPixel, BAR_WIDTH, barHeight);
    }
    case 3: {
        midRect = QRect(x, height() - data[1] * dataToPixel - 1, BAR_WIDTH, 2);
    }
    case 2: {
        double pixelMax = data.first() * dataToPixel;
        auto barHeight = pixelMax - data.last() * dataToPixel;
        barRect = QRect(x, height() - pixelMax, BAR_WIDTH, barHeight);
        break;
    }
    default:
        barRect = QRect(x, height() - data[0] * dataToPixel - 1, BAR_WIDTH, 2);
        break;
    }

    bool hovered = pointHovered(barRect);
    if (hovered) {
        //color = _highlightColor;
        color.setHsv(qAbs(color.hue() - 180), 255, 255);
    }

    painter.fillRect(barRect, color);
    painter.fillRect(upperRect, color.lighter());
    painter.fillRect(lowerRect, color.darker());
    painter.fillRect(midRect, color);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(barRect);
}


/**
 * @brief EntityChart::setSeriesVisible
 * @param kind
 * @param visible
 */
void EntityChart::setSeriesVisible(TIMELINE_SERIES_KIND kind, bool visible)
{
    _seriesKindVisible[kind] = visible;
    update();
}


/**
 * @brief EntityChart::setHovered
 * @param visible
 */
void EntityChart::setHovered(bool visible)
{
    _hovered = visible;
    if (!visible) {
        _hoveredPoints.clear();
        emit dataHovered(_hoveredPoints);
    }
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
 * @brief EntityChart::setHoveredRect
 * @param rect
 */
void EntityChart::setHoveredRect(QRectF rect)
{
    if (rect != _hoveredRect)
        _hoveredRect = rect;
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
 * @brief EntityChart::rangeChanged
 */
void EntityChart::rangeChanged()
{
    update();

    /*
    double dataRange = _dataMaxX - _dataMinX;
    double displayRange = _displayedMax - _displayedMin;
    if ((dataRange == 0) || (displayRange == 0)) {
        _xScale = 1;
    } else {
        _xScale = displayRange / dataRange;
    }
    mapPointsFromRange();
    */
}


/**
 * @brief EntityChart::mapPointsFromRange
 * This function effectively scales and translates the points based on the current chart transformations.
 * It maps the points so that they are respectively within the chart's visible region.
 * @param series
 * @param points
 */
void EntityChart::mapPointsFromRange(MEDEA::DataSeries* series, QList<QPointF> points)
{
    update();
    return;

    if (series) {
        TIMELINE_SERIES_KIND kind = series->getSeriesKind();
        if (points.isEmpty()) {
            points = series->getConstPoints();
            _mappedPoints.remove(kind);
            getSeriesHitRects(kind).clear();
        }
        QList<QPointF> mappedPoints;
        double w = getPointWidth(kind);
        int centerY = rect().center().y();
        for (QPointF p : points) {
            double x = mapValueFromRange(p.x(), _displayedMin, _displayedMax, (double) width());
            double y = centerY;
            if (series->isLineSeries() || series->isBarSeries()) {
                y = mapValueFromRange(p.y(), series->getRangeY().first, series->getRangeY().second, (double) height());
            }
            mappedPoints.append(QPointF(x, y));
            getSeriesHitRects(kind).insert(x, QRectF(x - w / 2.0, 0, w, height()));
        }
        _mappedPoints[kind] = mappedPoints;
    } else {
        for (MEDEA::DataSeries* s : _seriesList.values()) {
            mapPointsFromRange(s);
        }
    }    
    update();
}


/**
 * @brief EntityChart::mapValueFromRange
 * @param value
 * @param min
 * @param max
 * @param range
 * @return
 */
double EntityChart::mapValueFromRange(double value, double min, double max, double range)
{
    if ((max - min) == 0)
        return 0.0;

    double ratio = (value - min) / (max - min);
    return ratio * range;
}


/**
 * @brief EntityChart::pointHovered
 * @param kind
 * @param point
 * @param index
 * @return
 */
bool EntityChart::pointHovered(TIMELINE_SERIES_KIND kind, QPointF point, int index)
{
    if (_hoveredRect.isNull() || index == -1)
        return false;

    bool hovered = false;
    QRectF pointRect = getSeriesHitRects(kind).value(point.x());
    if (kind == TIMELINE_SERIES_KIND::BAR) {
        pointRect.setWidth(_barWidth);
        pointRect.moveCenter(QPointF(point.x(), pointRect.center().y()));
    }
    if (pointRect.intersects(_hoveredRect)) {
        MEDEA::DataSeries* series = _seriesList.value(kind, 0);
        if (series) {
            QList<QPointF> seriesPoints = series->getConstPoints();
            if (index >= 0 && index < seriesPoints.count()) {
                _hoveredPoints[kind].append(series->getConstPoints().at(index));
            }
        }
        hovered = true;
    }

    return hovered;
}


/**
 * @brief EntityChart::pointHovered
 * @param hitRect
 * @return
 */
bool EntityChart::pointHovered(const QRectF& hitRect)
{
    return _hoveredRect.intersects(hitRect);
}


/**
 * @brief EntityChart::getSeriesHitRects
 * @param kind
 * @return
 */
QHash<qint64, QRectF> &EntityChart::getSeriesHitRects(TIMELINE_SERIES_KIND kind)
{
    switch (kind) {
    case TIMELINE_SERIES_KIND::NOTIFICATION:
        return _notificationHitRects;
    case TIMELINE_SERIES_KIND::STATE:
        return _stateHitRects;
    case TIMELINE_SERIES_KIND::LINE:
        return _lineHitRects;
    case TIMELINE_SERIES_KIND::BAR:
        return _barHitRects;
    default:
        return _dataHitRects;
    }
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


/**
 * @brief EntityChart::getSeries
 * @return
 */
const QHash<TIMELINE_SERIES_KIND, MEDEA::DataSeries*>& EntityChart::getSeries()
{
    return _seriesList;
}
