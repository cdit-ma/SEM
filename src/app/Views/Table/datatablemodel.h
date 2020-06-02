#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include <QSet>
#include <QAbstractTableModel>

class ViewItem;
class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    enum ATTRIBUTE_ROLES {
        MULTILINE_ROLE = Qt::UserRole + 1,
        VALID_VALUES_ROLE = Qt::UserRole + 2,
        ID_ROLE = Qt::UserRole + 3,
        ICON_ROLE = Qt::UserRole + 4
    };
    
    explicit DataTableModel(ViewItem* item);
    ~DataTableModel() final;
	
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	void sort(int column, Qt::SortOrder order) override;
  
  int getIndex(const QString& keyName) const;

signals:
    void req_dataChanged(int ID, QString keyName, QVariant data);

public slots:
    void updatedData(const QString& keyName);
    void removedData(const QString& keyName);

    void addData(const QString& keyName);
    void clearData();

private:
    QString getKey(const QModelIndex &index) const;
    QString getKey(int row) const;
	QVariant getData(const QModelIndex &index) const;
	
	bool isIndexProtected(const QModelIndex &index) const;
	bool isRowProtected(int row) const;
	bool isDataProtected(int row) const;
	
	bool hasData() const;
	bool hasCodeEditor(const QModelIndex &index) const;
	bool hasIconEditor(const QModelIndex &index) const;

    void setupDataBinding();

    ViewItem* entity = nullptr;

    QList<QString> editableKeys;
    QList<QString> lockedKeys;
};

#endif // DATATABLEMODEL_H