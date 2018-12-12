#ifndef ENTITYSET_H
#define ENTITYSET_H



#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
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

    int getAllDepthChildrenCount();

    bool isExpanded();

    void setDepth(int depth);
    int getDepth();

    void setLabel(QString label);
    QString getLabel();

    void addChildEntitySet(EntitySet* child);
    void setParentEntitySet(EntitySet* parent);

    void setHovered(bool hovered);

signals:
    void childAdded();
    void childRemoved(EntitySet* child);

    void setChildVisible(bool visible);
    void visibilityChanged(bool visible);

    void hovered(bool state);

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
    int _ID;
    int _depth;
    QString _label;

    int allDepthChildrenCount = 0;
    bool _isExpanded = false;
    bool _isVisible = true;
    bool _isChildrenVisible = true;
    bool _axisLineVisible = true;
    bool _tickVisible = true;

    QPen _tickPen;
    int _tickLength;

    QLabel* textLabel;
    QLabel* iconLabel;
    QPixmap unExpandablePixmap;
    QPixmap expandedPixmap;
    QPixmap contractedPixmap;

    QString textColorStr;
    QString highlighColorStr;

    EntitySet* parentEntitySet = 0;
    QList<EntitySet*> childrenSets;
    //QHash<int, EntitySet*> childrenHash;
    QList<QtCharts::QAbstractSeries*> seriesList;

};

#endif // ENTITYSET_H
