#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H


#include "../../Controllers/ViewController/viewcontroller.h"

#include "datatablemodel.h"
#include "datatableview.h"
#include "datatablewidget.h"
#include "datatabledelegate.h"

#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QLabel>

class DataTableWidget : public QWidget
{
    Q_OBJECT
public:
    DataTableWidget(ViewController *controller, QWidget *parent = 0);

public slots:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);


private slots:
    void activeItem_IconChanged();
    void activeItem_LabelChanged();
    void themeChanged();
private:
    void setupLayout();


    ViewItem* activeItem;

    QSize iconSize;
    DataTableView* tableView;
    QToolBar* toolbar;
    QLabel* iconLabel;
    QLabel* label;
    ViewController* viewController;
    DataTableDelegate* multilineDelegate;

};

#endif // DATATABLEWIDGET_H
