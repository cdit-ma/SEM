#ifndef ENTITYSET_H
#define ENTITYSET_H

#include <QWidget>
#include <QLabel>
#include <QToolBar>
#include <QHBoxLayout>

#include "../../../../theme.h"
#include "entityaxis.h"

#include <QtCharts/QAbstractSeries>

class EntitySet : public QWidget
{
    friend class EntityAxis;
    Q_OBJECT

public:
    explicit EntitySet(QString label = "", QWidget* parent = 0);
    ~EntitySet();

    int getAllDepthChildrenCount() const;

    bool isExpanded();

    void setDepth(int depth);
    int getDepth() const;

    void setLabel(QString label);
    QString getLabel() const;

    void addChildEntitySet(EntitySet* child);
    void setParentEntitySet(EntitySet* parent);
    const QList<EntitySet*>& getChildrenEntitySets() const;

    void setHovered(bool hovered);

signals:
    void childAdded();
    void childRemoved(EntitySet* child);

    void setChildVisible(bool visible);
    void visibilityChanged(bool visible);

    void hovered(bool state);

    void closeEntity();

public slots:
    void themeChanged(Theme *theme);

    void toggleExpanded();
    void setVisible(bool visible);

    void childEntityAdded();
    void childEntityRemoved(EntitySet* child);

protected:
    bool eventFilter(QObject* object, QEvent* event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void paintEvent(QPaintEvent* event);

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

    QToolBar* toolbar_;
    QAction* closeAction_;
    QIcon closeIcon_;

    QLabel* textLabel_;
    QLabel* iconLabel_;

    QPixmap unExpandablePixmap_;
    QPixmap expandedPixmap_;
    QPixmap contractedPixmap_;

    QString textColorStr_;
    QString highlighColorStr_;

    EntitySet* parentEntitySet_ = 0;
    QList<EntitySet*> childrenSets_;
    QList<QtCharts::QAbstractSeries*> seriesList_;

};

#endif // ENTITYSET_H
