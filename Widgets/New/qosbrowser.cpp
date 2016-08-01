#include "qosbrowser.h"
#include <QBoxLayout>
#include "qosprofilemodel.h"
QOSBrowser::QOSBrowser(ViewController* vc, QWidget *parent) : QWidget(parent)
{
    qosModel = new QOSProfileModel(this);


    connect(vc, SIGNAL(viewItemConstructed(ViewItem*)), qosModel, SLOT(viewItem_Constructed(ViewItem*)));
    connect(vc, SIGNAL(viewItemDestructing(int,ViewItem*)), qosModel, SLOT(viewItem_Destructed(int,ViewItem*)));
    setupLayout();

    profileView->setModel(qosModel);
    elementView->setModel(qosModel);
    profileView->hideColumn(2);
    elementView->hideColumn(0);
    elementView->hideColumn(1);
    QItemSelectionModel* selectionModel = profileView->selectionModel();
    elementView->setSelectionModel(selectionModel);
}

void QOSBrowser::setupLayout()
{

    QHBoxLayout* layout = new QHBoxLayout(this);
    profileView = new QTreeView();
    elementView = new QTreeView();
    tableView = new QTableView();
    layout->addWidget(profileView);
    layout->addWidget(elementView);
    layout->addWidget(tableView);
}

