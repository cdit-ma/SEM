#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QObject>
#include <QAbstractButton>
#include <QToolButton>
#include <QToolBar>
#include <QGroupBox>
#include <QLayout>
#include <QBuffer>


/*
 * QVariantHasher Class
 * This class allows QVariants to be hashed.
 */
class QVariantHasher {
  public:
    QVariantHasher();
    uint hash(const QVariant & v);
  private:
    QByteArray bb;
    QBuffer buff;
    QDataStream ds;
};


class FilterGroup : public QObject
{
    Q_OBJECT
public:
    explicit FilterGroup(QString group, QObject *parent = 0);

    QString getFilterGroup();
    QGroupBox* constructFilterGroupBox(Qt::Orientation orientation = Qt::Vertical);

    void setExclusive(bool exclusive);
    void setResetButtonVisible(bool visible);
    void addToFilterGroup(QVariant key, QAbstractButton* filterButton);

signals:
    void filtersChanged(QList<QVariant> checkedKeys);
    void filtersCleared();

public slots:
    void themeChanged();
    void filterTriggered();
    void updateResetButtonVisibility();

private:
    void setupResetButton();
    QAction* addToGroupBox(QAbstractButton* button);

    void clearFilters();
    void updateFilterCheckedCount();

    QHash<QString, QAbstractButton*> filters;
    QGroupBox* filterGroupBox;
    QToolBar* filterToolbar;

    QToolButton* resetFilterButton;
    QAction* resetAction;
    bool showResetButton;

    QString filterGroup;
    bool exclusive;

    QHash<uint, QAbstractButton*> filterButtons;
    QList<QVariant> checkedKeys;
    QVariantHasher variantHasher;

};

#endif // FILTERGROUP_H
