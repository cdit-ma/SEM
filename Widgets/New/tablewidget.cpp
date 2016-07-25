#include "tablewidget.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QObject>
#include <QDebug>
#include <QPalette>
TableWidget::TableWidget(QWidget *parent) : QWidget(parent)
{

    iconSize = QSize(32,32);
    setupLayout();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    themeChanged();


    height = 300;


}

void TableWidget::activeSelectedItemChanged(ViewItem *item, bool isActive)
{
    QAbstractItemModel* model = 0;
    if(item){
        activeItem = item;
        model = item->getTableModel();

        QPair<QString, QString> iconPath = item->getIcon();

        QPixmap icon = Theme::theme()->getImage(iconPath.first, iconPath.second, iconSize);
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

void TableWidget::cyclePressed()
{
    if(sender() == cycleForwardAction){
        emit cycleActiveItem(true);
    }else if(sender() == cycleBackwardAction){
        emit cycleActiveItem(false);
    }
}

void TableWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    cycleBackwardAction->setIcon(theme->getIcon("Actions", "Arrow_Left"));
    cycleForwardAction->setIcon(theme->getIcon("Actions", "Arrow_Right"));

    QString BGColor = theme->getBackgroundColorHex();
    QString highlightColor = theme->getHighlightColorHex();
    QString altBGColor = theme->getAltBackgroundColorHex();
    QString disabledBGColor = theme->getDisabledBackgroundColorHex();
    QString textColor = theme->getTextColorHex();
    QString highlightTextColor = theme->getTextColorHex(Theme::CR_SELECTED);
    QString disabledTextColor = theme->getTextColorHex(Theme::CR_DISABLED);

    setStyleSheet("QLabel{color: "+ textColor + ";font-weight:bold;}"
                  "QTableView{background-color: " + BGColor + ";color: "+ textColor + ";}"
                  "QTableView::item::disabled{background-color: " + BGColor + ";color: " + disabledTextColor + ";}"
                  "QTableView::item::selected{background-color: " + highlightColor + ";color:" + highlightTextColor + ";}"
                  );
    tableView->setStyleSheet("QHeaderView{background-color: " + BGColor +";border:none;color:" + textColor+";}"
                "QHeaderView::section{background-color:" + altBGColor +";border:0px;}"
                "QHeaderView::section:hover{background-color:" + highlightColor +";color:" + highlightTextColor + ";border:0px;}"
                             "QHeaderView::section:selected{font-weight:normal;}"
                             "QTableView QLineEdit{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QComboBox{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QSpinBox {background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"
                             "QTableView QDoubleSpinBox{background-color: " + highlightColor + "; color: "+ highlightTextColor +";border:0px;}"


                );


    //tableView->setPalette(p);

}

void TableWidget::updateTableSize()
{
    if (tableView->model()) {
        qreal tableHeight = 0;
        tableHeight += tableView->horizontalHeader()->size().height();
        tableHeight += tableView->contentsMargins().top() + tableView->contentsMargins().bottom();
        for (int i = 0; i < tableView->model()->rowCount(); i++){
            tableHeight += tableView->rowHeight(i);
        }
        //tableView->resize(tableView->width(), tableHeight);
    } else {
        //tableView->resize(tableView->width(), 0);
    }
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

    QLabel* topLeftCornerLabel = new QLabel("Key", this);
    tableView->setCornerWidget(topLeftCornerLabel);
    iconLabel = new QLabel(this);
    label = new QLabel(this);
    label->setMinimumWidth(1);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    cycleBackwardAction = toolbar->addAction("Cycle Backward");
    toolbar->addWidget(iconLabel);
    toolbar->addWidget(label);
    cycleForwardAction = toolbar->addAction("Cycle Forward");

    layout->addWidget(toolbar);

    layout->addWidget(tableView, 1);
    //QWidget* widget = new QWidget(this);
    //widget->setStyleSheet("background:yellow;");
    //layout->addWidget(widget, 1);

    connect(cycleForwardAction, SIGNAL(triggered(bool)), this, SLOT(cyclePressed()));
    connect(cycleBackwardAction, SIGNAL(triggered(bool)), this, SLOT(cyclePressed()));
}
