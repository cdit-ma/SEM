#include "datatablewidget.h"
#include "../../../modelcontroller/keynames.h"

#include <QHeaderView>
#include <QStringBuilder>

DataTableWidget::DataTableWidget(ViewController *controller, QWidget *parent)
	: QWidget(parent)
{
    activeItem = nullptr;
    viewController = controller;
    
    setupLayout();
	setMinimumSize(130,130);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
	themeChanged();
    
    connect(viewController->getSelectionController(), &SelectionController::itemActiveSelectionChanged, this, &DataTableWidget::itemActiveSelectionChanged);
    connect(viewController, &ViewController::vc_editTableCell, tableView, &DataTableView::editDataValue);
    connect(entity_button, &QToolButton::clicked, this, &DataTableWidget::titleClicked);
}

void DataTableWidget::titleClicked()
{
    if (activeItem) {
        viewController->centerOnID(activeItem->getID());
    }
}

void DataTableWidget::itemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    QAbstractItemModel* model = nullptr;
    
    if (activeItem) {
        disconnect(activeItem, &ViewItem::iconChanged, this, &DataTableWidget::activeItem_IconChanged);
        disconnect(activeItem, &ViewItem::labelChanged, this, &DataTableWidget::activeItem_LabelChanged);
    }
    if (item && isActive) {
        connect(item, &ViewItem::iconChanged, this, &DataTableWidget::activeItem_IconChanged);
        connect(item, &ViewItem::labelChanged, this, &DataTableWidget::activeItem_LabelChanged);
        activeItem = item;
        model = item->getTableModel();
    } else {
        activeItem = nullptr;
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
        auto icon_path = activeItem->getIcon();
        entity_button->setIcon(Theme::theme()->getIcon(icon_path));
    }else{
        entity_button->setIcon(QIcon());
    }
}

void DataTableWidget::activeItem_LabelChanged()
{
    if (activeItem) {
        entity_button->setText(activeItem->getData(KeyName::Label).toString());
    } else {
        entity_button->setText("");
    }
}

void DataTableWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(Theme::theme()->getAbstractItemViewStyleSheet() +
                  "QAbstractItemView::item {"
                  "border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";"
                  "border-width: 1px 0px 1px 0px;"
                  "}"
                  );

    
    tableView->setStyleSheet("DataTableView{border:none;}");

    entity_button->setStyleSheet("QToolButton{background:rgba(0,0,0,0);color:" % theme->getTextColorHex() % ";border-color:rgba(0,0,0,0);font-weight:bold;}");
    toolbar->setIconSize(theme->getIconSize());
    activeItem_IconChanged();
}

void DataTableWidget::setupLayout()
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    toolbar = new QToolBar(this);

    tableView = new DataTableView(this);

    entity_button = new QToolButton(this);
    entity_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    auto left_stretch = new QWidget(this);
    auto right_stretch = new QWidget(this);
    left_stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionBackward->constructSubAction());
    toolbar->addWidget(left_stretch);
    toolbar->addWidget(entity_button);

    toolbar->addWidget(right_stretch);
    toolbar->addAction(viewController->getActionController()->edit_CycleActiveSelectionForward->constructSubAction());

    layout->addWidget(toolbar);
    layout->addWidget(tableView, 1);
}