#ifndef QOSPROFILEMODEL_H
#define QOSPROFILEMODEL_H

#include <QStandardItemModel>
#include <QObject>

#include "../../Controllers/ViewController/nodeviewitem.h"


class QOSModelItem;
class QOSProfileModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum QOS_ROLES {
        ID_ROLE = Qt::UserRole + 1,
        DATATABLE_ROLE = Qt::UserRole + 2,
        EDITABLELABEL_ROLE = Qt::UserRole + 3
    };
    QOSProfileModel(QObject *parent);

    QAbstractTableModel* getTableModel(const QModelIndex &index) const;
public slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
private:
    void removeItem(int ID);
    QStandardItem* rootItem;
    QHash<int, QOSModelItem*> modelItems;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

class QOSModelItem: public QObject, public QStandardItem{
    Q_OBJECT
public:
    QOSModelItem(NodeViewItem* item);
    NodeViewItem* getNodeViewItem();
    ~QOSModelItem();

    QVariant data(int role) const;

    void setData(const QVariant &value, int role);

signals:
    void dataChanged(int, QString, QVariant);
private slots:
    void itemChanged();
private:
    NodeViewItem* item;
    int ID;

};



#endif // QOSPROFILEMODEL_H
