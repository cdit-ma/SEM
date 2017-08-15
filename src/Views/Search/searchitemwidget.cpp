#include "searchitemwidget.h"

#include <QToolBar>

#define ICON_SIZE 32
#define MARGIN 5

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

    // this item is visible by default
    visible = true;

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
 * @brief SearchItemWidget::setAspectFilterKey
 * @param key
 */
void SearchItemWidget::setAspectFilterKey(int key)
{
    aspectFilterKey = key;
    filterVisibility[key] = true;
    updateVisibility(key, true);
}


/**
 * @brief SearchItemWidget::setDataFilterKey
 * @param key
 */
void SearchItemWidget::setDataFilterKey(int key)
{
    dataFilterKey = key;
    filterVisibility[key] = true;
    updateVisibility(key, true);
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
 * @brief SearchItemWidget::getViewAspect
 * @return
 */
VIEW_ASPECT SearchItemWidget::getViewAspect()
{
    return viewAspect;
}


/**
 * @brief SearchItemWidget::getDataKeys
 * @return
 */
QStringList SearchItemWidget::getDataKeys()
{
    return keys;
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
        arrowIcon.addPixmap(theme->getImage("Icons", "arrowHeadDown", QSize(), theme->getMenuIconColor()));
        arrowIcon.addPixmap(theme->getImage("Icons", "arrowHeadUp", QSize(), theme->getMenuIconColor()), QIcon::Normal, QIcon::On);
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
        keyWidgetsConstructed = true;
    }

    displayWidget->setVisible(checked);
    show();
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
        setSelected(!selected);
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
 * @param filterMatched
 */
void SearchItemWidget::updateVisibility(int filter, bool filterMatched)
{
    if (!filterVisibility.contains(filter)) {
        qWarning() << "SearchItemWidget::updateVisibility - Unknown filter.";
        return;
    }

    filterVisibility[filter] = filterMatched;
    bool showItem = filterMatched;

    if (visible != filterMatched) {
        if (filterMatched) {
            showItem = true;
            foreach (bool filterVisible, filterVisibility.values()) {
                if (!filterVisible) {
                    showItem = false;
                    break;
                }
            }
            // de-select this item if it is hidden
            if (!showItem && selected) {
                setSelected(false);
            }
        }
        setVisible(showItem);
        visible = showItem;
    }
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
