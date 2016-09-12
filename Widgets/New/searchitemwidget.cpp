#include "searchitemwidget.h"

#include <QVBoxLayout>
#include <QToolBar>

#define ICON_SIZE 24
#define MARGIN 5

/**
 * @brief SearchItemWidget::SearchItemWidget
 * @param item
 * @param parent
 */
SearchItemWidget::SearchItemWidget(ViewItem* item, QWidget *parent) : QFrame(parent)
{
    viewItem = item;
    viewItemID = -1;

    textLabel = new QLabel(this);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    keyWidgetsConstructed = false;
    doubleClicked = false;
    checkedKey = "All";

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(MARGIN);
    layout->setSpacing(0);

    if (item) {

        viewItemID = item->getID();
        textLabel->setText(item->getData("label").toString());

        iconSize = QSize(ICON_SIZE, ICON_SIZE);
        iconPath = item->getIcon();

        QPixmap itemPixmap = Theme::theme()->getImage(iconPath.first, iconPath.second, iconSize);
        if (itemPixmap.isNull()) {
            iconPath.first = "Actions";
            iconPath.second = "Help";
            itemPixmap = Theme::theme()->getImage("Actions", "Help", iconSize);
        }
        iconLabel = new QLabel(this);
        iconLabel->setPixmap(itemPixmap);
        iconLabel->setAlignment(Qt::AlignCenter);

        expandButton = new QToolButton(this);
        expandButton->setFixedSize(18, 18);
        expandButton->setCheckable(true);
        expandButton->setChecked(false);
        expandButton->setToolTip("Show/Hide Matching Data");

        centerButton = new QToolButton(this);
        centerButton->setFixedSize(18, 18);
        centerButton->setToolTip("Center View Aspect On Item");

        QToolBar* toolbar = new QToolBar(this);
        toolbar->setIconSize(QSize(18, 18));
        toolbar->addWidget(expandButton);
        toolbar->addWidget(centerButton);

        QWidget* topBarWidget = new QWidget(this);
        topBarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout* topLayout = new QHBoxLayout(topBarWidget);
        topLayout->setMargin(0);
        topLayout->setSpacing(MARGIN);
        topLayout->addWidget(iconLabel);
        topLayout->addWidget(textLabel, 1);
        topLayout->addWidget(toolbar);
        topBarWidget->setMinimumWidth(topLayout->sizeHint().width());

        displayWidget = new QWidget(this);
        displayWidget->setVisible(expandButton->isChecked());

        layout->addWidget(topBarWidget);
        layout->addWidget(displayWidget);

        connect(expandButton, SIGNAL(toggled(bool)), this, SLOT(expandButtonToggled(bool)));
        connect(centerButton, SIGNAL(clicked(bool)), this, SLOT(centerButtonClicked()));

    } else {
        iconLabel = 0;
        expandButton = 0;
        centerButton = 0;
        displayWidget = 0;
        textLabel->setText("No View Item");
        layout->addWidget(textLabel);
    }

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
    //updateColor(Theme::CR_NORMAL);
}


/**
 * @brief SearchItemWidget::setDisplayKeys
 * @param keys
 */
void SearchItemWidget::setDisplayKeys(QList<QString> keys)
{
    this->keys = keys;
}


/**
 * @brief SearchItemWidget::themeChanged
 */
void SearchItemWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame{ background:" + theme->getBackgroundColorHex() + "; color:" + theme->getTextColorHex() + "; border: 1px solid " + theme->getDisabledBackgroundColorHex() + ";}"
                  "QFrame:hover { background:" + theme->getDisabledBackgroundColorHex() + ";}"
                  "QPushButton{ background: rgba(0,0,0,0); border: 0px; }"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; }"
                  + theme->getToolBarStyleSheet());

    if (iconLabel) {
        iconLabel->setPixmap(theme->getImage(iconPath.first, iconPath.second, iconSize, theme->getMenuIconColor()));
    }
    if (expandButton) {
        expandButton->setIcon(theme->getIcon("Actions", "Arrow_Down"));
    }
    if (centerButton) {
        centerButton->setIcon(theme->getIcon("Actions", "Crosshair"));
    }
}


/**
 * @brief SearchItemWidget::centerButtonClicked
 */
void SearchItemWidget::centerButtonClicked()
{
    emit centerOnViewItem(viewItemID);
}


/**
 * @brief SearchItemWidget::expandButtonToggled
 * @param checked
 */
void SearchItemWidget::expandButtonToggled(bool checked)
{
    hide();

    // construct the key widgets when the item is expanded for the first time
    // only show the key widgets that match the currently checked key
    if (!keyWidgetsConstructed) {
        constructKeyWidgets();
        toggleKeyWidget(checkedKey);
        keyWidgetsConstructed = true;
    }

    displayWidget->setVisible(checked);
    show();
}


/**
 * @brief SearchItemWidget::toggleKeyWidget
 * @param key
 */
void SearchItemWidget::toggleKeyWidget(QString key)
{
    checkedKey = key;

    if (!keys.contains(key) && key != "All") {
        setVisible(false);
        return;
    }

    if (key == "All") {
        foreach (QWidget* w, keyWidgetHash.values()) {
            w->setVisible(true);
        }
    } else {
        foreach (QString widgetKey, keyWidgetHash.keys()) {
            QWidget* w = keyWidgetHash.value(widgetKey);
            bool showWidget = widgetKey == key;
            w->setVisible(showWidget);
        }
    }
    setVisible(true);
}


/**
 * @brief SearchItemWidget::mouseReleaseEvent
 * Imitate clicking the expand button when any of this widget is clicked.
 */
void SearchItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (doubleClicked) {
        if (expandButton) {
            bool checked = !expandButton->isChecked();
            expandButton->setChecked(checked);
            expandButtonToggled(checked);
        }
        doubleClicked = false;
    } else {
        //emit centerOnViewItem(viewItemID);
    }
}


/**
 * @brief SearchItemWidget::mouseDoubleClickEvent
 */
void SearchItemWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    doubleClicked = true;
}


/**
 * @brief SearchItemWidget::enterEvent
 */
void SearchItemWidget::enterEvent(QEvent *)
{
    updateColor(Theme::CR_SELECTED);
    emit hoverEnter(viewItemID);
}


/**
 * @brief SearchItemWidget::leaveEvent
 */
void SearchItemWidget::leaveEvent(QEvent *)
{
    updateColor(Theme::CR_NORMAL);
    emit hoverLeave(viewItemID);
}


/**
 * @brief SearchItemWidget::updateColor
 * @param colorRole
 */
void SearchItemWidget::updateColor(Theme::COLOR_ROLE colorRole)
{
    return;

    Theme* theme = Theme::theme();
    textLabel->setStyleSheet("color:" + theme->getTextColorHex(colorRole) + ";");
    if (viewItem) {
        displayWidget->setStyleSheet("color:" + theme->getTextColorHex(colorRole) + ";");
        iconLabel->setPixmap(theme->getImage(iconPath.first, iconPath.second, iconSize, theme->getMenuIconColor(colorRole)));
        //expandButton->setIcon(QIcon(Theme::theme()->getImage("Actions", "Arrow_Down", QSize(), theme->getTextColorHex(colorRole))));
    }
}


/**
 * @brief SearchItemWidget::constructKeyWidgets
 */
void SearchItemWidget::constructKeyWidgets()
{
    if (!keys.isEmpty()) {

        QVBoxLayout* displayLayout = new QVBoxLayout(displayWidget);
        displayLayout->setMargin(0);
        displayLayout->setSpacing(2);

        foreach (QString key, keys) {
            QWidget* keyWidget = new QWidget(this);
            keyWidget->setStyleSheet("background: rgba(0,0,0,0);");
            keyWidgetHash[key] = keyWidget;

            QLabel* keyLabel = new QLabel(key + ":", this);
            QLabel* valueLabel = new QLabel("ViewItem is null.", this);
            if (viewItem) {
                valueLabel->setText(viewItem->getData(key).toString());
            }

            QHBoxLayout* layout = new QHBoxLayout(keyWidget);
            layout->setMargin(0);
            layout->setSpacing(5);
            if (iconLabel) {
                layout->addSpacerItem(new QSpacerItem(iconLabel->sizeHint().width() + MARGIN, 0));
            }
            layout->addWidget(keyLabel);
            layout->addWidget(valueLabel, 1);
            displayLayout->addWidget(keyWidget);
        }
    }
}

