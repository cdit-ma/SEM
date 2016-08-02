#include "tablewidget.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QObject>
#include <QDebug>
#include <QPalette>
#include <QStringBuilder>

TableWidget::TableWidget(ViewController *controller, QWidget *parent) : QWidget(parent)
{
    viewController = controller;
    multilineDelegate = new MultilineDelegate(this);
    iconSize = QSize(32,32);
    setupLayout();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    themeChanged();
}

void TableWidget::itemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    QAbstractItemModel* model = 0;
    if(item){
        activeItem = item;
        model = item->getTableModel();

        QPair<QString, QString> iconPath = item->getIcon();

        QPixmap icon = Theme::theme()->getImage(iconPath.first, iconPath.second, iconSize);
        //icon = icon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(icon);
        label->setText(item->getData("label").toString());
    }else{
        iconLabel->setPixmap(QPixmap());
        iconLabel->setText("");
        label->setText("");
    }

    if (tableView) {
        tableView->setModel(model);
        tableView->resizeColumnsToContents();
        //tableView->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    }
}

void TableWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    label->setStyleSheet("QLabel{color: " % theme->getTextColorHex() % ";font-weight:bold;}");
    setStyleSheet(Theme::theme()->getAbstractItemViewStyleSheet());
    toolbar->setStyleSheet("QToolBar{ border: 1px solid " % theme->getDisabledBackgroundColorHex() % "; border-bottom: 0px; }");
}

void TableWidget::setupLayout()
{
    layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);

    toolbar = new QToolBar(this);
    toolbar->setFixedHeight(iconSize.height());
    tableView = new QTableView(this);
    tableView->horizontalHeader()->setVisible(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tableView->verticalHeader()->setHighlightSections(false);

    //tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    //tableView->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);
    //tableView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);


    tableView->setItemDelegate(multilineDelegate);

    iconLabel = new QLabel(this);
    label = new QLabel(this);

    QWidget* labelWidget = new QWidget(this);

    QHBoxLayout* labelWidgetLayout = new QHBoxLayout(labelWidget);
    labelWidgetLayout->setMargin(0);
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
