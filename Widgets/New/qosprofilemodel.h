#ifndef QOSPROFILEMODEL_H
#define QOSPROFILEMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include "../../View/nodeviewitem.h"
class QOSProfileModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    QOSProfileModel(QObject *parent);

private slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

private:
    QHash<int, NodeViewItem*> viewItems;
    QList<int> profiles;
    QHash<int, QList<int> > settings;

    // QAbstractItemModel interface
public:
    ViewItem* getViewItem(const QModelIndex& index) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
};

#endif // QOSPROFILEMODEL_H
