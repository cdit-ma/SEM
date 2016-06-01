#ifndef XMITREEMODEL_H
#define XMITREEMODEL_H
#include <QStandardItem>
#include <QObject>
enum XMI_Roles{
    XMI_NAME = Qt::UserRole + 1,
    XMI_ID = Qt::UserRole + 2,
    XMI_TYPE = Qt::UserRole + 3,
    XMI_CHECKED = Qt::UserRole + 4
};

class XMITreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    XMITreeModel(QObject* parent = 0);
    QStringList getSelectedClassIDs();
signals:
    void selectionChanged(Qt::CheckState overallState);
public slots:
    void selectAll();
    void selectNone();
public:
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
private:
    QList<QModelIndex> getChildren(const QModelIndex &index = QModelIndex(), int depth = -1) const;

    // QAbstractItemModel interface
public:
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // XMITREEMODEL_H
