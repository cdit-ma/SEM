#include "datatablewidget.h"
#include "../../theme.h"

#include <QHeaderView>
#include <QObject>
#include <QDebug>
#include <QStringBuilder>

DataTableWidget::DataTableWidget(ViewController *controller, QWidget *parent) : QWidget(parent)
{
    activeItem = 0;
    viewController = controller;
    multilineDelegate = new DataTableDelegate(this);
    setupLayout();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    themeChanged();


    connect(viewController, &ViewController::vc_editTableCell, tableView, &DataTableView::editDataValue);
}

void DataTableWidget::itemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    QAbstractItemModel* model = 0;
    if(activeItem){
        disconnect(activeItem, &ViewItem::iconChanged, this, &DataTableWidget::activeItem_IconChanged);
        disconnect(activeItem, &ViewItem::labelChanged, this, &DataTableWidget::activeItem_LabelChanged);
    }
    if(item && isActive){
        connect(item, &ViewItem::iconChanged, this, &DataTableWidget::activeItem_IconChanged);
        connect(item, &ViewItem::labelChanged, this, &DataTableWidget::activeItem_LabelChanged);
        activeItem = item;
        model = item->getTableModel();
    }else{
        activeItem = 0;
    }
    activeItem_IconChanged();
    activeItem_LabelChanged();

    if (tableView) {
        tableView->setModel(model);
    }
}

void DataTableWidget::activeItem_IconChanged()
{
    if(activeItem){
        QPair<QString, QString> iconPath = activeItem->getIcon();
        entity_button->setIcon(Theme::theme()->getIcon(iconPath));//.pixmap(24,24);
    }else{
        entity_button->setIcon(QIcon());
    }
}

void DataTableWidget::activeItem_LabelChanged()
{
    if(activeItem){
        entity_button->setText(activeItem->getData("label").toString());
    }else{
        entity_button->setText("");
    }
}

void DataTableWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(Theme::theme()->getAbstractItemViewStyleSheet() +
                  "QAbstractItemView::item {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "border-width: 0px 0px 1px 0px;"
                  "}");
    //label->setStyleSheet("QLabel{color: " % theme->getTextColorHex() % ";font-weight:bold;}");
    toolbar->setStyleSheet("QToolBar{ border: 1px solid " % theme->getDisabledBackgroundColorHex() % "; border-bottom: 0px; }");
    entity_button->setStyleSheet("QToolButton{background:rgba(0,0,0,0);color:" % theme->getTextColorHex() % ";border-color:rgba(0,0,0,0);font-size:10px;font-weight:bold;}");
    activeItem_IconChanged();
}

void DataTableWidget::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    toolbar = new QToolBar(this);
    //toolbar->setFixedHeight(32);

    toolbar->setIconSize(QSize(16,16));
    tableView = new DataTableView(this);

    //iconLabel = new QLabel(this);
    //label = new QLabel(this);

   // QWidget* labelWidget = new QWidget(this);

    entity_button = new QToolButton(this);
    entity_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
   // entity_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //entity_button->setEnabled(false);

    auto left_stretch = new QWidget(this);
    auto right_stretch = new QWidget(this);
    left_stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
/*
    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidgetLayout->setSpacing(0);
    
    
    auto left_stretch = new QWidget(this);
    auto right_stretch = new QWidget(this);
    left_stretch->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);
    right_stretch->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    labelWidgetLayout->addStretch();
    labelWidgetLayout->addWidget(iconLabel);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();


    label->setAlignment(Qt::AlignCenter);*/

    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionBackward->constructSubAction());
    toolbar->addWidget(left_stretch);
    toolbar->addWidget(entity_button);
    toolbar->addWidget(right_stretch);
    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionForward->constructSubAction());

    layout->addWidget(toolbar);

    layout->addWidget(tableView, 1);
}
