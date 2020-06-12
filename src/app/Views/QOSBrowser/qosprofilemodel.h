#ifndef QOSPROFILEMODEL_H
#define QOSPROFILEMODEL_H

#include <QStandardItemModel>

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
    
    explicit QOSProfileModel(QObject *parent);
    
    QAbstractTableModel* getTableModel(const QModelIndex &index) const;
    
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
public slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
    
private:
    QHash<int, QOSModelItem*> modelItems;
};


class QOSModelItem : public QObject, public QStandardItem
{
    Q_OBJECT

public:
    explicit QOSModelItem(NodeViewItem* item);
    ~QOSModelItem() final;
    
    QVariant data(int role) const override;
    void setData(const QVariant &value, int role) override;

signals:
    void dataChanged(int, QString, QVariant);
    
private slots:
    void itemChanged();

private:
    NodeViewItem* nodeViewItem_ = nullptr;
    int ID;
};

#endif // QOSPROFILEMODEL_H