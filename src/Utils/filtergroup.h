#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include "../theme.h"
#include "customgroupbox.h"

#include <QObject>
#include <QAbstractButton>
#include <QPushButton>
#include <QToolButton>
#include <QToolBar>
#include <QGroupBox>
#include <QLayout>
#include <QBuffer>

class FilterGroup : public QObject
{
    Q_OBJECT
public:
    explicit FilterGroup(QString title, QVariant groupKey = QVariant(), QObject *parent = 0);

    CustomGroupBox* constructFilterBox();

    QGroupBox* constructFilterGroupBox(Qt::Orientation orientation = Qt::Vertical);
    QVariant getFilterGroupKey();
    QVariant getResetKey();

    QList<QVariant> getFilterKeys();
    QList<QVariant> getCheckedFilterKeys();

    void setExclusive(bool exclusive);
    void setVisible(bool visible);

    void setResetButtonVisible(bool visible);
    void setResetButtonText(QString text);
    void setResetButtonKey(QVariant key);

    void addFilterToolButton(QVariant key, QString label, QString iconPath, QString iconName);
    void addFilterPushButton(QVariant key, QString label, QString iconPath, QString iconName);
    void addToFilterGroup(QVariant key, QAbstractButton* filterButton);

    void clearFilterGroup();

signals:
    void filtersChanged(QList<QVariant> checkedKeys);
    void filtersCleared();

public slots:
    void themeChanged();
    void resetFilters();

    void updateResetButtonVisibility();

private slots:
    void filterTriggered();

private:
    void setupResetButton();

    void addFilterButton(QAbstractButton *button, QVariant key, QString label, QString iconPath, QString iconName);
    QAction* addToGroupBox(QAbstractButton* button);

    void addToFilterBox(QAbstractButton* button);

    void clearFilters();
    void updateFilterCheckedCount();

    CustomGroupBox* customFilterBox;

    QGroupBox* filterGroupBox;
    QToolBar* filterToolbar;

    QToolButton* resetButton;
    QAction* resetAction;
    QVariant resetKey;
    bool showResetButton;

    QVariant filterGroupKey;
    QString filterGroupTitle;
    bool exclusive;

    QHash<QVariant, QAbstractButton*> filterButtonsHash;
    QList<QVariant> checkedKeys;

};

inline uint qHash(QVariant key, uint seed)
{
    return ::qHash(key.toUInt(), seed);
}
Q_DECLARE_METATYPE(QVariant)

#endif // FILTERGROUP_H
