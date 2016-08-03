#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QVBoxLayout>
#include <QToolBar>

#include <QTableView>
#include <QLabel>
#include "../../View/Table/attributetableview.h"
#include "../../View/viewitem.h"
#include "../../Controller/viewcontroller.h"
#include "../../View/Table/multilinedelegate.h"
class TableWidget : public QWidget
{
    Q_OBJECT
public:
    TableWidget(ViewController *controller, QWidget *parent = 0);

private slots:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);
    void themeChanged();
private:
    void setupLayout();


    ViewItem* activeItem;

    QSize iconSize;
    AttributeTableView* tableView;
    QToolBar* toolbar;
    QLabel* iconLabel;
    QLabel* label;
    ViewController* viewController;
    AttributeTableDelegate* multilineDelegate;

};

#endif // TABLEWIDGET_H
