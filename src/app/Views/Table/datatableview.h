#ifndef DATATABLEVIEW_H
#define DATATABLEVIEW_H

#include <QTableView>

class DataTableView : public QTableView
{
    Q_OBJECT
    
public:
    explicit DataTableView(QWidget* parent = nullptr);
    
    void setModel(QAbstractItemModel* model) override;
    
public slots:
    void editDataValue(int ID, const QString& keyName);
};

#endif // DATATABLEVIEW_H