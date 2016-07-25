#include "tablewidget.h"
#include "../../View/theme.h"
#include <QHeaderView>
#include <QObject>
#include <QDebug>

TableWidget::TableWidget(QWidget *parent) : QWidget(parent)
{

    iconSize = QSize(32,32);
    setupLayout();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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

        if (tableView->horizontalHeader()->count() == 2) {
            //Set the resize mode of the sections on first model launch.
            tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
            tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        }
        tableView->resizeColumnToContents(0);

        updateTableSize();
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

    QString textColor = theme->getTextColorHex();
    setStyleSheet("QLabel{color: "+ textColor + ";font-weight:bold;}");
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
        tableView->resize(tableView->width(), tableHeight);
    } else {
        tableView->resize(tableView->width(), 0);
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

    connect(cycleForwardAction, SIGNAL(triggered(bool)), this, SLOT(cyclePressed()));
    connect(cycleBackwardAction, SIGNAL(triggered(bool)), this, SLOT(cyclePressed()));
}
