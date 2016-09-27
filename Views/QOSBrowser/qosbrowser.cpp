#include "qosbrowser.h"

#include <QBoxLayout>
#include <QHeaderView>
#include <QDebug>
#include "qosprofilemodel.h"
#include "../../theme.h"

QOSBrowser::QOSBrowser(ViewController* vc, QWidget *parent) : QWidget(parent)
{
    this->vc = vc;
    qosModel = new QOSProfileModel(this);
    setStyleSheet("QOSBrowser{padding:0px;margin:0px;}");


    connect(vc, &ViewController::vc_viewItemConstructed, qosModel, &QOSProfileModel::viewItem_Constructed);
    connect(vc, &ViewController::vc_viewItemDestructing, qosModel, &QOSProfileModel::viewItem_Destructed);


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
    profileLabelButton->setStyleSheet("text-align: left; border-radius: 0px; background:" + theme->getAltBackgroundColorHex() + ";");
}

void QOSBrowser::profileSelected(QModelIndex index1, QModelIndex)
{
    if(index1.isValid()){
        elementView->setModel(qosModel);
        elementView->setSelectionModel(elementViewSelectionModel);
        elementView->setRootIndex(index1);
    }else{
        elementView->setModel(0);
    }
    removeSelection->setEnabled(index1.isValid());
}

void QOSBrowser::settingSelected(QModelIndex index1, QModelIndex)
{
    tableView->setModel(qosModel->getTableModel(index1));
}

void QOSBrowser::removeSelectedProfile()
{
    QList<int> IDs;
    foreach(QModelIndex index, profileView->selectionModel()->selectedIndexes()){
        int ID = index.data(QOSProfileModel::ID_ROLE).toInt();
        if(ID > 0){
            IDs.append(ID);
        }
    }
    if(!IDs.isEmpty()){
        vc->vc_deleteEntities(IDs);
    }
}

void QOSBrowser::setupLayout()
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(5);
    layout->setMargin(0);

    horizontalSplitter = new QSplitter(this);
    profileView = new QListView(this);
    elementView = new QTreeView(this);
    tableView = new DataTableView(this);
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(20,20));

    //profileView->setFocusPolicy(Qt::NoFocus);
    //elementView->setFocusPolicy(Qt::NoFocus);

    profileView->setAttribute(Qt::WA_MacShowFocusRect, false);
    elementView->setAttribute(Qt::WA_MacShowFocusRect, false);

    toolbar->addAction(vc->getActionController()->toolbar_addDDSQOSProfile);
    removeSelection = vc->getActionController()->toolbar_removeDDSQOSProfile;

    toolbar->addAction(removeSelection);
    connect(removeSelection, &QAction::triggered, this, &QOSBrowser::removeSelectedProfile);
    removeSelection->setEnabled(false);

    profileLabelButton = new QPushButton("Profiles", this);
    profileLabelButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    profileLabelButton->setEnabled(false);
    profileLabelButton->setFixedHeight(elementView->header()->height() - 5);

    QWidget* profileWidget = new QWidget(this);
    profileWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QVBoxLayout* profileLayout = new QVBoxLayout(profileWidget);
    profileLayout->setSpacing(5);
    profileLayout->setMargin(0);
    profileLayout->addWidget(profileLabelButton);
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

