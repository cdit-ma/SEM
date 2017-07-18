#include "searchitemwidget.h"

#include <QToolBar>

#define ICON_SIZE 32
#define MARGIN 5

#define ASPECT_FILTER "Aspect"
#define DATA_FILTER "Data"

/**
 * @brief SearchItemWidget::SearchItemWidget
 * @param item
 * @param parent
 */
SearchItemWidget::SearchItemWidget(ViewItem* item, QWidget *parent)
    : QFrame(parent)
{
    viewItem = item;
    viewItemID = -1;

    if (viewItem->isNode()) {
        NodeViewItem* nvi = (NodeViewItem*)viewItem;
        viewAspect = nvi->getViewAspect();
    }

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
        displayWidget = 0;
        layout->addWidget(textLabel);
    }

    // this item is visible by default - initialise all filter visibility to true
    //filterVisibility[ASPECT_FILTER] = true;
    //filterVisibility[DATA_FILTER] = true;

    setSelected(false);
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
 * @brief SearchItemWidget::setAspectFilterKey
 * @param key
 */
void SearchItemWidget::setAspectFilterKey(QVariant key)
{
    aspectFilterKey = key;
    filterVisibility[key.toString()] = true;
}


/**
 * @brief SearchItemWidget::setDataFilterKey
 * @param key
 */
void SearchItemWidget::setDataFilterKey(QVariant key)
{
    dataFilterKey = key;
    filterVisibility[key.toString()] = true;
}


/**
 * @brief SearchItemWidget::setFilterKeys
 * @param keys
 */
void SearchItemWidget::setFilterKeys(QList<QVariant> keys)
{
    // this item is visible by default - initialise all filter visibility to true
    foreach (QVariant key, keys) {
        filterVisibility[key.toString()] = true;
    }
}


/**
 * @brief SearchItemWidget::setSelected
 * @param selected
 */
void SearchItemWidget::setSelected(bool selected)
{
    if (this->selected == selected) {
        return;
    }

    this->selected = selected;

    if (selected) {
        emit itemSelected(viewItemID);
        backgroundColor =  Theme::theme()->getAltBackgroundColorHex();
    } else {
        backgroundColor =  Theme::theme()->getBackgroundColorHex();
    }

    updateStyleSheet();
}


/**
 * @brief SearchItemWidget::themeChanged
 */
void SearchItemWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    if (selected) {
        backgroundColor =  theme->getAltBackgroundColorHex();
    } else {
        backgroundColor = theme->getBackgroundColorHex();
    }

    updateStyleSheet();

    if (iconLabel) {
        iconLabel->setPixmap(theme->getImage(iconPath.first, iconPath.second, iconSize, theme->getMenuIconColor()));
    }

    if (expandButton) {
        QIcon arrowIcon;
        arrowIcon.addPixmap(theme->getImage("Icon", "arrowHeadDown", QSize(), theme->getMenuIconColor()));
        arrowIcon.addPixmap(theme->getImage("Icon", "arrowHeadUp", QSize(), theme->getMenuIconColor()), QIcon::Normal, QIcon::On);
        expandButton->setIcon(arrowIcon);
        expandButton->setStyleSheet("QToolButton{ background: rgba(0,0,0,0); border: 0px; }");
    }
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
 * @brief SearchItemWidget::filterCleared
 * @param filter
 */
void SearchItemWidget::filterCleared(QVariant filter)
{
    updateVisibility(filter, true);
}


/**
 * @brief SearchItemWidget::filtersChanged
 * @param filter
 * @param checkedKeys
 */
void SearchItemWidget::filtersChanged(QVariant filter, QList<QVariant> checkedKeys)
{
    bool visible = true;
    if (filter == aspectFilterKey) {
        int aspectInt = static_cast<int>(viewAspect);
        qDebug() << "INT - " << aspectInt;
        visible = checkedKeys.contains(aspectInt);
    } else if (filter == dataFilterKey) {
        foreach (QVariant key, checkedKeys) {
            if (!keys.contains(key.toString())) {
                visible = false;
                break;
            }
        }
    }
    updateVisibility(filter, visible);
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
        setSelected(true);
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

    //QSize toolButtonSize(18, 18);
    QSize toolButtonSize(24, 24);

    expandButton = new QToolButton(this);
    expandButton->setFixedSize(toolButtonSize);
    expandButton->setCheckable(true);
    expandButton->setChecked(false);
    expandButton->setEnabled(false);
    expandButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    expandButton->setToolTip("Double-Click To Show/Hide Matching Data");

    QToolBar* toolbar = new QToolBar(this);
    toolbar->setIconSize(toolButtonSize);
    toolbar->addWidget(expandButton);

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
}


/**
 * @brief SearchItemWidget::constructKeyWidgets
 */
void SearchItemWidget::constructKeyWidgets()
{
    if (!keys.isEmpty()) {

        QVBoxLayout* displayLayout = new QVBoxLayout(displayWidget);
        displayLayout->setMargin(MARGIN);
        displayLayout->setSpacing(MARGIN*2);

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
            layout->setSpacing(MARGIN);
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


/**
 * @brief SearchItemWidget::updateStyleSheet
 */
void SearchItemWidget::updateStyleSheet()
{
    Theme* theme = Theme::theme();
    setStyleSheet("QFrame {"
                  "border-style: solid;"
                  "border-width: 0px 0px 1px 0px;"
                  "border-color:" + theme->getDisabledBackgroundColorHex() + ";"
                  "background:" + backgroundColor + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QFrame:hover { background:" + theme->getDisabledBackgroundColorHex() + ";}"
                  "QLabel{ background: rgba(0,0,0,0); border: 0px; }"
                  "QLabel#KEY_LABEL{ color:" + theme->getAltTextColorHex() + ";}"
                  + theme->getToolBarStyleSheet());
}


/**
 * @brief SearchItemWidget::updateVisibility
 * @param filter
 * @param visible
 */
void SearchItemWidget::updateVisibility(QVariant filter, bool visible)
{
    if (!filterVisibility.contains(filter.toString())) {
        return;
    }

    filterVisibility[filter.toString()] = visible;
    if (isVisible() != visible) {
        bool allVisible = true;
        foreach (bool visible, filterVisibility.values()) {
            if (!visible) {
                allVisible = false;
                break;
            }
        }
        if (isVisible() != allVisible) {
            setVisible(allVisible);
            // de-select this item if it is hidden
            if (!allVisible && selected) {
                setSelected(false);
            }
        }
    }
}

