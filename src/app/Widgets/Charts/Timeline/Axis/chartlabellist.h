#ifndef CHARTLABELLIST_H
#define CHARTLABELLIST_H

#include <QWidget>
#include <QVBoxLayout>

namespace MEDEA {

class ChartLabel;

/**
 * @brief The ChartLabelList class holds and displays an ordered list of ChartLabels.
 * This class holds the ChartLabel widgets in order of insertion.
 * It is also in charge of setting whether a "tick" will be rendered on the right hand side of its children ChartLabels.
 */
class ChartLabelList : public QWidget
{
    Q_OBJECT

public:
    explicit ChartLabelList(QWidget* parent = nullptr);

    int appendChartLabel(ChartLabel* chartLabel);
    int insertChartLabel(ChartLabel* prevChartLabel, ChartLabel* chartLabel);
    int removeChartLabel(ChartLabel* chartLabel);

    int getChartLabelIndex(ChartLabel* chartLabel) const;

    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);

signals:
    void sizeChanged(QSizeF size);

public slots:
    void themeChanged();
    void resizeEvent(QResizeEvent* event);

private:
    QVBoxLayout* mainLayout_ = nullptr;

    int minimumEntityHeight_ = 0;
    int maximumEntityWidth_ = 0;

    bool axisLineVisible_ = true;
    bool tickVisible_ = true;
};

}

#endif // CHARTLABELLIST_H
