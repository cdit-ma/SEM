#ifndef ATTRIBUTETABLEMODEL_H
#define ATTRIBUTETABLEMODEL_H

#include <QAbstractTableModel>


#include "../../Model/data.h"
#include "../../Model/graphml.h"
#include "../GraphicsItems/graphmlitem.h"
#include <QVector>

class NodeItem;
class EdgeItem;
class GraphMLItem;

class AttributeTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AttributeTableModel(EntityAdapter* adapter);
    ~AttributeTableModel();

signals:
    void req_dataChanged(int ID, QString keyName, QVariant data);
    void editMultilineData(Data* data);
public slots:
    void updatedData(QString keyName);
    void removedData(QString keyName);

    void addData(QString keyName);
    void clearData();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int row, int count, const QModelIndex &parent);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    void sort(int column, Qt::SortOrder order);
private:
    int getIndex(QString keyName) const;
    QString getKey(const QModelIndex &index) const;
    QString getKey(int row) const;
    bool isIndexProtected(const QModelIndex &index) const;
    bool isRowProtected(int row) const;
    bool hasPopupEditor(const QModelIndex &index) const;
    QVariant getData(const QModelIndex &index) const;

    bool isDataProtected(int row) const;
    bool hasData() const;

    void setupDataBinding();

    EntityAdapter* entity;





    QStringList editableKeys;
    QStringList lockedKeys;
    QStringList keyOrder;

};

#endif // ATTRIBUTETABLEMODEL_H
