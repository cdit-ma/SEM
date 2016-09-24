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
        QPixmap icon = Theme::theme()->getImage(iconPath.first, iconPath.second, QSize(32,32));
        iconLabel->setPixmap(icon);
    }else{
        iconLabel->clear();
    }
}

void DataTableWidget::activeItem_LabelChanged()
{
    if(activeItem){
        label->setText(activeItem->getData("label").toString());
    }else{
        label->clear();
    }
}

void DataTableWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(Theme::theme()->getAbstractItemViewStyleSheet());
    label->setStyleSheet("QLabel{color: " % theme->getTextColorHex() % ";font-weight:bold;}");
    toolbar->setStyleSheet("QToolBar{ border: 1px solid " % theme->getDisabledBackgroundColorHex() % "; border-bottom: 0px; }");
}

void DataTableWidget::setupLayout()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    toolbar = new QToolBar(this);
    toolbar->setFixedHeight(32);
    toolbar->setIconSize(QSize(20,20));
    tableView = new DataTableView(this);

    iconLabel = new QLabel(this);
    label = new QLabel(this);

    QWidget* labelWidget = new QWidget(this);

    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
    labelWidgetLayout->setSpacing(0);
    labelWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    labelWidgetLayout->addStretch();
    labelWidgetLayout->addWidget(iconLabel);
    labelWidgetLayout->addWidget(label);
    labelWidgetLayout->addStretch();


    label->setAlignment(Qt::AlignCenter);

    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionBackward->constructSubAction());
    toolbar->addWidget(labelWidget);
    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionForward->constructSubAction());

    layout->addWidget(toolbar);

    layout->addWidget(tableView, 1);
}
