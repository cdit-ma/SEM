#include "qosbrowser.h"
#include <QBoxLayout>
#include "qosprofilemodel.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QDebug>
QOSBrowser::QOSBrowser(ViewController* vc, QWidget *parent) : QWidget(parent)
{
    qosModel = new QOSProfileModel(this);


    setStyleSheet("QOSBrowser{padding:5px;margin:0px;}");
    connect(vc, SIGNAL(viewItemConstructed(ViewItem*)), qosModel, SLOT(viewItem_Constructed(ViewItem*)));
    connect(vc, SIGNAL(viewItemDestructing(int,ViewItem*)), qosModel, SLOT(viewItem_Destructed(int,ViewItem*)));
    setupLayout();

    profileView->setModel(qosModel);
    elementView->setModel(qosModel);
    profileView->setSelectionBehavior(QAbstractItemView::SelectItems);
    profileView->setSelectionMode(QAbstractItemView::SingleSelection);
    elementView->setSelectionBehavior(QAbstractItemView::SelectItems);
    elementView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(profileView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(profileSelected(QModelIndex, QModelIndex)));
    //
    connect(elementView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(settingSelected(QModelIndex, QModelIndex)));

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
}

void QOSBrowser::themeChanged()
{
    Theme* theme = Theme::theme();
    profileView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    elementView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    tableView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
}

void QOSBrowser::profileSelected(QModelIndex index1, QModelIndex index2)
{
    int ID = qosModel->data(index1, QOSProfileModel::ID_ROLE).toInt();
    qCritical() << ID;
    elementView->setRootIndex(index1);
    //NodeViewItem* item = qosModel->getNodeViewItem(ID);
}

void QOSBrowser::settingSelected(QModelIndex index1, QModelIndex)
{
    tableView->setModel(qosModel->getTableModel(index1));
}

void QOSBrowser::setupLayout()
{

    QHBoxLayout* layout = new QHBoxLayout(this);
    profileView = new QListView();
    elementView = new QListView();
    tableView = new QTableView();
    tableView->horizontalHeader()->setVisible(false);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->verticalHeader()->setHighlightSections(false);

    layout->addWidget(profileView,1);
    layout->addWidget(elementView,1);
    layout->addWidget(tableView,2);
}

