#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableView>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QLabel>
#include <QAction>
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

    QVBoxLayout * layout;
    ViewItem* activeItem;

    QSize iconSize;
    QTableView* tableView;
    QToolBar* toolbar;
    QLabel* iconLabel;
    QLabel* label;
    ViewController* viewController;
    MultilineDelegate* multilineDelegate;

};

#endif // TABLEWIDGET_H
