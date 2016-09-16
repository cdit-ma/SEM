#include "searchitemwidget.h"

#include <QToolBar>

#define ICON_SIZE 32
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

    keyWidgetsConstructed = false;
    doubleClicked = false;
    checkedKey = "All";

    textLabel = new QLabel("No View Item", this);
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    textLabel->setFont(QFont(font().family(), 10));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(0);

    if (viewItem) {
        viewItemID = viewItem->getID();
        viewItem->registerObject(this);
        textLabel->setText(viewItem->getData("label").toString());
        iconPath = viewItem->getIcon();
        setupLayout(layout);
    } else {
        iconLabel = 0;
        expandButton = 0;
        centerButton = 0;
        popupButton = 0;
        displayWidget = 0;
        layout->addWidget(textLabel);
    }

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


/**
 * @brief SearchItemWidget::~SearchItemWidget
 */
SearchItemWidget::~SearchItemWidget()
{
    if (viewItem) {
        viewItem->unregisterObject(this);
    }
}


/**
 * @brief SearchItemWidget::addDisplayKey
 * @param key
 */
void SearchItemWidget::addDisplayKey(QString key)
{
    if (!keys.contains(key)) {
        keys.append(key);
    }
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
                  "QLabel#KEY_LABEL{ color:" + theme->getAltTextColorHex() + ";}"
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
    if (popupButton) {
        popupButton->setIcon(theme->getIcon("Actions", "Popup"));
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
 * @brief SearchItemWidget::popupButtonClicked
 */
void SearchItemWidget::popupButtonClicked()
{
    emit popupViewItem(viewItemID);
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
    emit hoverEnter(viewItemID);
}


/**
 * @brief SearchItemWidget::leaveEvent
 */
void SearchItemWidget::leaveEvent(QEvent *)
{
    emit hoverLeave(viewItemID);
}


/**
 * @brief SearchItemWidget::setupLayout
 * @param layout
 */
void SearchItemWidget::setupLayout(QVBoxLayout* layout)
{
    iconSize = QSize(ICON_SIZE, ICON_SIZE);

    QPixmap itemPixmap = Theme::theme()->getImage(iconPath.first, iconPath.second, iconSize);
    if (itemPixmap.isNull()) {
        iconPath.first = "Actions";
        iconPath.second = "Help";
        itemPixmap = Theme::theme()->getImage("Actions", "Help", iconSize);
    }

    iconLabel = new QLabel(this);
    iconLabel->setPixmap(itemPixmap);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFixedSize(itemPixmap.size() + QSize(MARGIN, MARGIN));

    QSize toolButtonSize(18, 18);

    expandButton = new QToolButton(this);
    expandButton->setFixedSize(toolButtonSize);
    expandButton->setCheckable(true);
    expandButton->setChecked(false);
    expandButton->setToolTip("Show/Hide Matching Data");

    centerButton = new QToolButton(this);
    centerButton->setFixedSize(toolButtonSize);
    centerButton->setToolTip("Center View Aspect On Item");

    popupButton = new QToolButton(this);
    popupButton->setFixedSize(toolButtonSize);
    popupButton->setToolTip("View Item In New Window");

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(toolButtonSize);
    toolbar->addWidget(expandButton);
    toolbar->addWidget(centerButton);
    toolbar->addWidget(popupButton);

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
    connect(popupButton, SIGNAL(clicked(bool)), this, SLOT(popupButtonClicked()));
}


/**
 * @brief SearchItemWidget::constructKeyWidgets
 */
void SearchItemWidget::constructKeyWidgets()
{
    if (!keys.isEmpty()) {

        QVBoxLayout* displayLayout = new QVBoxLayout(displayWidget);
        displayLayout->setMargin(MARGIN);
        displayLayout->setSpacing(2);

        foreach (QString key, keys) {
            QWidget* keyWidget = new QWidget(this);
            keyWidget->setStyleSheet("background: rgba(0,0,0,0);");
            keyWidgetHash[key] = keyWidget;

            QLabel* keyLabel = new QLabel(key + ":", this);
            keyLabel->setObjectName("KEY_LABEL");

            QLabel* valueLabel = new QLabel("ViewItem is null.", this);
            if (viewItem) {
                //valueLabel->setText("<i>" + viewItem->getData(key).toString() + "</i>");
                valueLabel->setText(viewItem->getData(key).toString());
            }

            QHBoxLayout* layout = new QHBoxLayout(keyWidget);
            layout->setMargin(0);
            layout->setSpacing(5);
            if (iconLabel) {
                layout->addSpacerItem(new QSpacerItem(iconLabel->sizeHint().width(), 0));
            }
            layout->addWidget(keyLabel);
            layout->addWidget(valueLabel, 1);
            displayLayout->addWidget(keyWidget);
        }

        displayLayout->addSpacerItem(new QSpacerItem(0, MARGIN));
    }
}

