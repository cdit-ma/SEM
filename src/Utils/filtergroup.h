#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QObject>
#include <QAbstractButton>
#include <QToolButton>
#include <QToolBar>
#include <QGroupBox>
#include <QLayout>
#include <QBuffer>

class FilterGroup : public QObject
{
    Q_OBJECT
public:
    explicit FilterGroup(QString group, QObject *parent = 0);

    QString getFilterGroup();
    QGroupBox* constructFilterGroupBox(Qt::Orientation orientation = Qt::Vertical);

    void setExclusive(bool exclusive);
    void setResetButtonVisible(bool visible);
    void setResetButtonKey(QVariant key);
    void addToFilterGroup(QVariant key, QAbstractButton* filterButton);

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
    QAction* addToGroupBox(QAbstractButton* button);

    void clearFilters();
    void updateFilterCheckedCount();

    QHash<QString, QAbstractButton*> filters;
    QGroupBox* filterGroupBox;
    QToolBar* filterToolbar;

    QToolButton* resetButton;
    QAction* resetAction;
    QVariant resetKey;
    bool showResetButton;

    QString filterGroup;
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
