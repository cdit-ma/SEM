#ifndef FILTERGROUP_H
#define FILTERGROUP_H

#include <QObject>
#include <QAbstractButton>
#include <QToolButton>
#include <QToolBar>
#include <QGroupBox>
#include <QLayout>
#include <QBuffer>


class QVariantHasher {
  public:
    QVariantHasher() : buff(&bb), ds(&buff) {
      bb.reserve(1000);
      buff.open(QIODevice::WriteOnly);
    }
    uint hash(const QVariant & v) {
      buff.seek(0);
      ds << v;
      return qHashBits(bb.constData(), buff.pos());
    }
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
    void addToFilterGroup(QString key, QAbstractButton* filterButton);
    void addToFilterGroup(QVariant key, QAbstractButton* filterButton);

signals:
    void filtersChanged(QStringList keys);
    void resetFilterGroup();

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

    QStringList checkedKeys;
    QString filterGroup;
    bool exclusive;

    //QHash<QVariant, QAbstractButton*> filterButtons;
    QHash<uint, QAbstractButton*> filterButtons;
    QVariantHasher variantHasher;
    QList<QVariant> checkedFilterKeys;

};

#endif // FILTERGROUP_H
