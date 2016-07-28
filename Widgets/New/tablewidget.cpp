#include "tablewidget.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QObject>
#include <QDebug>
#include <QPalette>

TableWidget::TableWidget(ViewController *controller, QWidget *parent) : QWidget(parent)
{
    viewController = controller;
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
    }
}

void TableWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    QString BGColor = theme->getBackgroundColorHex();
    QString highlightColor = theme->getHighlightColorHex();
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString disabledBGColor = theme->getDisabledBackgroundColorHex();
    QString textColor = theme->getTextColorHex();
    QString highlightTextColor = theme->getTextColorHex(Theme::CR_SELECTED);
    QString disabledTextColor = theme->getTextColorHex(Theme::CR_DISABLED);

    setStyleSheet("QLabel{color: "+ textColor + ";font-weight:bold;}"
                  "QTableView{background-color: " + BGColor + ";color: "+ textColor + "; border: 1px solid " + disabledBGColor + ";}" //border: 0px; }"
                  "QTableView::item::disabled{background-color: " + BGColor + ";color: " + disabledTextColor + ";}"
                  "QTableView::item::selected{background-color: " + highlightColor + ";color:" + highlightTextColor + ";}"
                  );
    tableView->setStyleSheet("QHeaderView{background-color: " + BGColor +";border:none;color:" + textColor+";}"
                "QHeaderView::section{background-color:" + altBGColor +"; border: 1px solid " + BGColor + "; padding: 0px 0px 0px 3px;}"
                "QHeaderView::section:hover{background-color:" + highlightColor +";color:" + highlightTextColor + ";border:0px;}"
                             "QHeaderView::section:selected{font-weight:normal;}"
                             "QTableView QLineEdit{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QComboBox{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QSpinBox {background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QDoubleSpinBox{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                );

    toolbar->setStyleSheet("QToolBar{ border: 1px solid " + disabledBGColor + "; border-bottom: 0px; }");

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
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->verticalHeader()->setHighlightSections(false);

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
