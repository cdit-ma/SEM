#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QObject>
#include <QAbstractButton>

class FilterGroup : public QObject
{
    Q_OBJECT
public:
    explicit FilterGroup(QString group, QObject *parent = 0);

    QString getFilterGroup();
    void setExclusive(bool exclusive);
    void addToFilterGroup(QString key, QAbstractButton* filterButton, bool resetFilterButton = false);

signals:
    void filtersChanged(QStringList keys);
    void resetFilterGroup();

public slots:
    void filterTriggered();

private:
    void clearFilters();

    QHash<QString, QAbstractButton*> filters;
    QAbstractButton* resetFilterButton;

    QStringList checkedKeys;
    QString filterGroup;
    bool exclusive;

};

#endif // FILTERGROUP_H
