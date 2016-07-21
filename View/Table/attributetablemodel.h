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
    AttributeTableModel(GraphMLItem* guiItem, QObject* parent = 0);
    AttributeTableModel(EntityAdapter* adapter);
    ~AttributeTableModel();

signals:
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
private:
    int getIndex(QString keyName) const;
    QVariant getDataValue(int row) const;
    QString getKeyName(int row) const;
    bool isDataProtected(int row) const;
    bool hasData() const;

    void setupDataBinding();
    bool popupMultiLine(const QModelIndex &index) const;
    GraphMLItem* guiItem;

    bool isNode;
    EntityAdapter* attachedEntity;

    QStringList keys;
    QStringList dataOrder;

    QStringList permanentlyLockedKeyNames;
    QStringList hiddenKeyNames;
    QStringList multiLineKeyNames;
};

#endif // ATTRIBUTETABLEMODEL_H
