#include "qosbrowser.h"
#include <QBoxLayout>
#include "qosprofilemodel.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QDebug>
QOSBrowser::QOSBrowser(ViewController* vc, QWidget *parent) : QWidget(parent)
{
    this->vc = vc;
    qosModel = new QOSProfileModel(this);
    setStyleSheet("QOSBrowser{padding:0px;margin:0px;}");

    connect(vc, SIGNAL(viewItemConstructed(ViewItem*)), qosModel, SLOT(viewItem_Constructed(ViewItem*)));
    connect(vc, SIGNAL(viewItemDestructing(int,ViewItem*)), qosModel, SLOT(viewItem_Destructed(int,ViewItem*)));

    setupLayout();




    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

void QOSBrowser::themeChanged()
{
    Theme* theme = Theme::theme();
    horizontalSplitter->setStyleSheet(theme->getSplitterStyleSheet());
    profileView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    elementView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
    tableView->setStyleSheet(theme->getAbstractItemViewStyleSheet());
}

void QOSBrowser::profileSelected(QModelIndex index1, QModelIndex index2)
{
    if(index1.isValid()){
        elementView->setModel(qosModel);
        elementView->setSelectionModel(elementViewSelectionModel);
        elementView->setRootIndex(index1);
    }else{
        elementView->setModel(0);
    }
}

void QOSBrowser::settingSelected(QModelIndex index1, QModelIndex)
{
    tableView->setModel(qosModel->getTableModel(index1));
}

void QOSBrowser::setupLayout()
{

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(5);
    layout->setMargin(0);

    horizontalSplitter = new QSplitter(this);
    profileView = new QListView(this);
    elementView = new QTreeView(this);
    tableView = new AttributeTableView(this);
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));
    toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    toolbar->addAction(vc->getActionController()->toolbar_addDDSQOSProfile);

    QWidget* profileWidget = new QWidget(this);
    profileWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout* profileLayout = new QVBoxLayout(profileWidget);
    profileLayout->setSpacing(5);
    profileLayout->setMargin(0);
    profileLayout->addWidget(profileView, 1);
    profileLayout->addWidget(toolbar, 0, Qt::AlignRight);
    elementView->header()->setMinimumHeight(25);

    horizontalSplitter->addWidget(profileWidget);
    horizontalSplitter->addWidget(elementView);
    horizontalSplitter->addWidget(tableView);
    layout->addWidget(horizontalSplitter);

    profileView->setModel(qosModel);
    elementView->setModel(0);

    elementViewSelectionModel = new QItemSelectionModel(qosModel);

    profileView->setSelectionBehavior(QAbstractItemView::SelectItems);
    profileView->setSelectionMode(QAbstractItemView::SingleSelection);

    elementView->setSelectionBehavior(QAbstractItemView::SelectItems);
    elementView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(profileView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(profileSelected(QModelIndex, QModelIndex)));
    connect(elementViewSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(settingSelected(QModelIndex, QModelIndex)));
}

