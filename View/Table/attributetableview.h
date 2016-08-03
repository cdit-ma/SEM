#ifndef ATTRIBUTETABLEVIEW_H
#define ATTRIBUTETABLEVIEW_H

#include <QTableView>
class AttributeTableView : public QTableView
{
    Q_OBJECT
public:
    AttributeTableView(QWidget *parent=0);
    void setModel(QAbstractItemModel *model);
};

#endif // ATTRIBUTETABLEVIEW_H
