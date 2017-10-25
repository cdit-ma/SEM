#ifndef DATATABLEWIDGET_H
#define DATATABLEWIDGET_H


#include "../../Controllers/ViewController/viewcontroller.h"

#include "datatablemodel.h"
#include "datatableview.h"
#include "datatablewidget.h"
#include "datatabledelegate.h"

#include <QToolButton>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QLabel>

class DataTableWidget : public QWidget
{
    Q_OBJECT
public:
    DataTableWidget(ViewController *controller, QWidget *parent = 0);

private slots:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);

private slots:
    void activeItem_IconChanged();
    void activeItem_LabelChanged();
    void themeChanged();
    void titleClicked();
private:
    void setupLayout();


    ViewItem* activeItem;

    QSize iconSize;
    DataTableView* tableView;
    QToolBar* toolbar;
    QToolButton* entity_button;
    
    ViewController* viewController;
    DataTableDelegate* multilineDelegate;

};

#endif // DATATABLEWIDGET_H
