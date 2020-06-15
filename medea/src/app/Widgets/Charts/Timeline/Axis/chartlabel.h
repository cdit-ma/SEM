#ifndef CHARTLABEL_H
#define CHARTLABEL_H

#include <QWidget>
#include <QLabel>
#include <QToolBar>
#include <QHBoxLayout>
#include <QPen>

#include "../../../../theme.h"
#include "chartlabellist.h"

namespace MEDEA {

/**
 * @brief The ChartLabel class is used to display the label of a Chart.
 * This class displays an icon (non-expandable/expanded/contracted), a label and a close button linked to a Chart.
 * It can contain children ChartLabels to provide a visual hierarchy/grouping, and a close button that when triggered, sends a signal to close the corresponding chart.
 */
class ChartLabel : public QWidget
{
    friend class ChartLabelList;
    Q_OBJECT

public:
    explicit ChartLabel(const QString& label = "", QWidget* parent = nullptr);
    ~ChartLabel() final;

    int getAllDepthChildrenCount() const;

    bool isExpanded();

    void setDepth(int depth);
    int getDepth() const;

    void setLabel(const QString& label);
    QString getLabel() const;

    void addChildChartLabel(ChartLabel* child);
    const QList<ChartLabel*>& getChildrenChartLabels() const;

    void setHovered(bool hovered);

signals:
    void childAdded();
    void childRemoved(ChartLabel* child);

    void setChildVisible(bool visible);
    void visibilityChanged(bool visible);

    void hovered(bool state);

    void closeChart();

public slots:
    void themeChanged(Theme *theme);

    void toggleExpanded();
    void setVisible(bool visible) override;

    void childLabelAdded();
    void childLabelRemoved(ChartLabel* child);

protected:
    bool eventFilter(QObject* object, QEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);

private:
    int ID_;
    int depth_;
    QString label_;

    int allDepthChildrenCount_ = 0;
    bool isExpanded_ = false;
    bool isVisible_ = true;
    bool isChildrenVisible_ = true;
    bool axisLineVisible_ = true;
    bool tickVisible_ = true;

    QPen tickPen_;
    int tickLength_;

    QToolBar* toolbar_ = nullptr;
    QAction* closeAction_ = nullptr;
    QIcon closeIcon_;

    QLabel* textLabel_ = nullptr;
    QLabel* expandedIconLabel_ = nullptr;

    QPixmap unExpandablePixmap_;
    QPixmap expandedPixmap_;
    QPixmap contractedPixmap_;

    QString textColorStr_;
    QString highlighColorStr_;

    QList<ChartLabel*> childrenChartLabels_;
};

}

#endif // CHARTLABEL_H