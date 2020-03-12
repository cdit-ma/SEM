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
    DataTableWidget(ViewController *controller, QWidget *parent = nullptr);

private slots:
    void itemActiveSelectionChanged(ViewItem* item, bool isActive);

    void activeItem_IconChanged();
    void activeItem_LabelChanged();
    void themeChanged();
    void titleClicked();
    
private:
    void setupLayout();

    ViewItem* activeItem = nullptr;

    DataTableView* tableView = nullptr;
    QToolBar* toolbar = nullptr;
    QToolButton* entity_button = nullptr;
    
    ViewController* viewController = nullptr;
};

#endif // DATATABLEWIDGET_H