#ifndef TIMELINECHARTVIEW_H
#define TIMELINECHARTVIEW_H

#include "../../../../Controllers/ViewController/viewitem.h"
#include "../../Timeline/Chart/hoverpopup.h"
#include "../Axis/chartlabellist.h"
#include "../Axis/chartlabel.h"
#include "chart.h"

#include "../../Data/Events/portlifecycleevent.h"
#include "../../Data/Events/workloadevent.h"
#include "../../Data/Events/cpuutilisationevent.h"
#include "../../Data/Events/memoryutilisationevent.h"
#include "../../Data/Events/markerevent.h"
#include "../../Data/Events/portevent.h"
#include "../../Data/Events/networkutilisationevent.h"

#include "../../Data/Series/portlifecycleeventseries.h"
#include "../../Data/Series/workloadeventseries.h"
#include "../../Data/Series/cpuutilisationeventseries.h"
#include "../../Data/Series/memoryutilisationeventseries.h"
#include "../../Data/Series/markereventseries.h"
#include "../../Data/Series/porteventseries.h"
#include "../../Data/Series/networkutilisationeventseries.h"
#include "../../Data/experimentrundata.h"

#include <QWidget>
#include <QToolBar>
#include <QPushButton>
#include <QScrollArea>
#include <QMultiHash>

enum class VALUE_TYPE{DOUBLE, DATE_TIME};
enum class TIME_DISPLAY_FORMAT{VALUE, DATE_TIME, ELAPSED_TIME};

class AxisWidget;
class TimelineChartView : public QWidget
{
    Q_OBJECT

public:
    explicit TimelineChartView(QWidget* parent = nullptr);

    bool eventFilter(QObject *watched, QEvent* event) override;

    void setTimeDisplayFormat( TIME_DISPLAY_FORMAT format);

    void addChart(const QPointer<const MEDEA::EventSeries>& series, const MEDEA::ExperimentRunData& exp_run_data);
    void removeChart(const QString& id, bool clearing_chart_list = false);

    void clearChartList();

signals:
    void seriesLegendHovered(MEDEA::ChartDataKind kind);

public slots:
    void themeChanged();

    void toggledSeriesLegend(bool checked);

    void chartLabelListSizeChanged(QSizeF size);
    void chartHovered(MEDEA::Chart* chart, bool hovered);
    void chartClosed();

    void updateHoverDisplay();
    void updateExperimentRunLastUpdatedTime(quint32 experimentRunID, qint64 time);

private slots:
    void minSliderMoved(double ratio);
    void maxSliderMoved(double ratio);

    void timelineZoomed(int delta);
    void timelinePanned(double dx, double dy);
    void timelineRubberbandUsed(double left, double right);
    
private:
    void setTimeRangeForExperimentRun(quint32 experimentRunID, qint64 startTime, qint64 lastUpdatedTime);
    void decrementSeriesCountForExperimentRun(quint32 experimentRunID);
    void updateTimelineRange(bool updateDisplayRange = true);

    void chartsEmptied();

    const QString& getDateTimeDisplayFormat(const MEDEA::ChartDataKind& kind) const;

    void setupLayout();

    QLabel* emptyLabel_ = nullptr;
    QWidget* mainWidget_ = nullptr;

    MEDEA::ChartList* chartList_ = nullptr;
    MEDEA::ChartLabelList* chartLabelList_ = nullptr;
    AxisWidget* timelineAxis_ = nullptr;
    HoverPopup* hoverDisplay_ = nullptr;

    QToolBar* legendToolbar_ = nullptr;
    QScrollArea* scrollArea_ = nullptr;

    QWidget* topFillerWidget_ = nullptr;
    QWidget* bottomFillerWidget_ = nullptr;
    QWidget* hoverWidget_ = nullptr;

    TIME_DISPLAY_FORMAT timeDisplayFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;

    QHash<MEDEA::ChartDataKind, QAction*> legendActions_;
    QHash<MEDEA::ChartDataKind, QPushButton*> hoverDisplayButtons_;

    QHash<qint32, int> experimentRunSeriesCount_;
    QHash<quint32, QPair<qint64, qint64>> experimentRunTimeRange_;
    QPair<quint32, qint64> longestExperimentRunDuration_;
    QPair<qint64, qint64> totalTimeRange_;

    QHash<QString, MEDEA::ChartLabel*> chartLabels_;
    QHash<QString, MEDEA::Chart*> charts_;

    static const MEDEA::ChartDataKind no_data_kind_;
};

#endif // TIMELINECHARTVIEW_H