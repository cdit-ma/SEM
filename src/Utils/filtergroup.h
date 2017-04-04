#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QObject>
#include <QAbstractButton>
#include <QToolButton>
#include <QToolBar>
#include <QGroupBox>
#include <QLayout>

class FilterGroup : public QObject
{
    Q_OBJECT
public:
    explicit FilterGroup(QString group, QObject *parent = 0);

    QString getFilterGroup();
    QGroupBox* constructFilterGroupBox(Qt::Orientation orientation = Qt::Vertical);

    void setExclusive(bool exclusive);
    void setResetButtonVisible(bool visible);
    void addToFilterGroup(QString key, QAbstractButton* filterButton);

signals:
    void filtersChanged(QStringList keys);
    void resetFilterGroup();

public slots:
    void themeChanged();
    void filterTriggered();
    void updateResetButtonVisibility();

private:
    void setupResetButton();
    void addToGroupBox(QAbstractButton* button);

    void clearFilters();
    void updateFilterCheckedCount();

    QHash<QString, QAbstractButton*> filters;
    QGroupBox* filterGroupBox;
    QToolBar* filterToolbar;

    QToolButton* resetFilterButton;
    QAction* resetAction;
    bool showResetButton;

    QStringList checkedKeys;
    QString filterGroup;
    bool exclusive;

};

#endif // FILTERGROUP_H
