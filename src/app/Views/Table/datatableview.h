#ifndef DATATABLEVIEW_H
#define DATATABLEVIEW_H

#include <QTableView>
class DataTableView : public QTableView
{
    Q_OBJECT
public:
    DataTableView(QWidget *parent=0);
    void setModel(QAbstractItemModel *model);
public slots:
    void editDataValue(int ID, QString keyName);
};

#endif // DATATABLEVIEW_H
