#include "toolbarwidgetaction.h"
#include "toolbarwidgetmenu.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param nodeKind
 * @param textLabel
 * @param parent
 */
ToolbarWidgetAction::ToolbarWidgetAction(QString nodeKind, QString textLabel, ToolbarWidgetMenu* parent) :
    QWidgetAction(parent)
{
    actionButton = 0;

    parentMenu = parent;
    nodeItem = 0;
    actionButton = 0;
    kind = nodeKind;
    label = nodeKind;

    widgetMenu = 0;
    willHaveMenu = false;
    deletable = true;
    parentMenuHasFocus = false;

    if (kind == "ComponentInstance" || kind == "ComponentImpl" || kind == "BlackBoxInstance" ||
            kind == "InEventPortDelegate" || kind == "OutEventPortDelegate") {
        willHaveMenu = true;
        deletable = false;
    } else if (kind == "Info") {
        label = textLabel;
        deletable = false;
        setEnabled(false);
    }
}


/**
 * @brief ToolbarWidgetAction::ToolbarWidgetAction
 * @param nodeItem
 * @param parent
 * @param willHaveMenu
 */
ToolbarWidgetAction::ToolbarWidgetAction(NodeItem* nodeItem, ToolbarWidgetMenu* parent, bool willHaveMenu) :
    QWidgetAction(parent)
{
    actionButton = 0;

    if (nodeItem) {
        kind = nodeItem->getNodeKind();
        label =  nodeItem->getNodeLabel();
        if (label == "") {
            label = kind;
        }
    }

    this->nodeItem = nodeItem;
    this->willHaveMenu = willHaveMenu;
    parentMenu = parent;
    widgetMenu = 0;
    deletable = true;
    parentMenuHasFocus = false;
}


/**
 * @brief ToolbarWidgetAction::setMenu
 * @param menu
 */
void ToolbarWidgetAction::setMenu(ToolbarWidgetMenu* menu)
{
    // disconnect the old menu
    prevWidgetMenu = widgetMenu;
    if (prevWidgetMenu) {
        disconnect(this, SIGNAL(triggered()), prevWidgetMenu, SLOT(execMenu()));
        disconnect(prevWidgetMenu, SIGNAL(aboutToHide()), this, SLOT(resetActionButton()));
    }
    // connect the new menu
    widgetMenu = menu;
    if (widgetMenu) {
        connect(this, SIGNAL(triggered()), widgetMenu, SLOT(execMenu()));
        connect(widgetMenu, SIGNAL(aboutToHide()), this, SLOT(resetActionButton()));
    }
}


/**
 * @brief ToolbarWidgetAction::getMenu
 * @return
 */
ToolbarWidgetMenu* ToolbarWidgetAction::getMenu()
{
    return widgetMenu;
}


/**
 * @brief ToolbarWidgetAction::getButton
 * @return
 */
ToolbarAbstractButton* ToolbarWidgetAction::getButton()
{
    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::getButtonPos
 * This is used to determine where to draw the menu for this action.
 * @return
 */
QPoint ToolbarWidgetAction::getButtonPos()
{
    return actionButton->mapToGlobal(actionButton->rect().topRight());
}


/**
 * @brief ToolbarWidgetAction::getNodeItem
 * @return
 */
NodeItem* ToolbarWidgetAction::getNodeItem()
{
    return nodeItem;
}


/**
 * @brief ToolbarWidgetAction::getNodeItemID
 * @return
 */
int ToolbarWidgetAction::getNodeItemID()
{
    if (nodeItem) {
        return nodeItem->getID();
    }
    return -1;
}


/**
 * @brief ToolbarWidgetAction::getActionKind
 * This action's kind is its nodeitem's kind if it has one, or "info" if it doesn't.
 * If kind is "info" it means that it is a default action, and all it is used for is
 * to display information about why its parent menu is empty.
 * @return
 */
QString ToolbarWidgetAction::getActionKind()
{
    return kind;
}


/**
 * @brief ToolbarWidgetAction::isDeletable
 * @return
 */
bool ToolbarWidgetAction::isDeletable()
{
    return deletable;
}


/**
 * @brief ToolbarWidgetAction::getParentMenu
 * @return
 */
ToolbarWidgetMenu* ToolbarWidgetAction::getParentMenu()
{
    return parentMenu;
}


/**
 * @brief ToolbarWidgetAction::getTopMostParentAction
 * This returns this ToolbarWidgetAction's top most parent ToolbarWidgetAction.
 * @return
 */
ToolbarWidgetAction* ToolbarWidgetAction::getTopMostParentAction()
{
    if (parentMenu) {

        ToolbarWidgetMenu* topMenu = parentMenu;
        ToolbarWidgetAction* topAction = 0;
        ToolbarWidgetAction* prevTopAction = 0;

        while (topAction = topMenu->getParentAction()) {
            topMenu = topAction->getParentMenu();
            if (!topMenu) {
                break;
            }
            prevTopAction = topAction;
        }

        return prevTopAction;
    }

    return 0;
}


/**
 * @brief ToolbarWidgetAction::createWidget
 * @param parent
 * @return
 */
QWidget* ToolbarWidgetAction::createWidget(QWidget* parent)
{
    actionButton = new ToolbarAbstractButton(parent);
    actionButton->setMinimumHeight(33);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);

    QString actionKind = getActionKind();
    if (actionKind == "HardwareNode"){
        Node* node = nodeItem->getNode();
        if (node) {
            if(node->getDataValue("localhost") == "true"){
                actionKind = "Localhost";
            }else{
                actionKind = node->getDataValue("os") + "_" + node->getDataValue("architecture");
            }
        }
    }

    QString alias = "Items";
    if (actionKind == "Info") {
        alias = "Actions";
    }

    QImage* image = new QImage(":/" + alias + "/" + actionKind + ".png");
    QImage scaledImage = image->scaled(actionButton->height(),
                                       actionButton->height(),
                                       Qt::KeepAspectRatio,
                                       Qt::SmoothTransformation);

    QLabel* imageLabel = new QLabel(actionButton);
    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->setFixedSize(scaledImage.size());
    imageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    QLabel* textLabel = new QLabel(label, actionButton);
    QFont font = textLabel->font();
    font.setPointSizeF(8.5);
    textLabel->setFixedHeight(25);
    textLabel->setFont(font);
    textLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    layout->addSpacerItem(new QSpacerItem(5,0));
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    actionButton->setLayout(layout);

    int minWidth = textLabel->fontMetrics().width(label) + imageLabel->width();

    // if this action is going to have a menu, add menu_arrow icon to the widget
    if (willHaveMenu) {
        QImage* menuImage = new QImage(":/Actions/Arrow_Right.png");
        QImage scaledMenuImage = menuImage->scaled(actionButton->height()/2.5,
                                                   actionButton->height()/2.5,
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation);
        QLabel* menuImageLabel = new QLabel(actionButton);
        menuImageLabel->setPixmap(QPixmap::fromImage(scaledMenuImage));
        menuImageLabel->setFixedSize(scaledMenuImage.size());
        menuImageLabel->setStyleSheet("background-color: rgba(0,0,0,0);");
        layout->addWidget(menuImageLabel);
        minWidth += menuImageLabel->width();
    }

    actionButton->setMinimumWidth(minWidth + 30);

    // make connections
    connect(this, SIGNAL(hovered()), this, SLOT(hover()));

    // only connect these signals and slots if this action is enabled
    if (isEnabled()) {
        connect(actionButton, SIGNAL(pressed()), this, SLOT(actionButtonPressed()));
        connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
        //connect(actionButton, SIGNAL(toolButton_checked()), this, SLOT(actionButtonClicked()));
    }

    if (parentMenu) {
        connect(parentMenu, SIGNAL(toolbarMenu_setFocus(bool)), this, SLOT(setParentMenuFocus(bool)));
        connect(parentMenu, SIGNAL(toolbarMenu_resetActions()), this, SLOT(resetActionButton()));
    } else {
        qDebug() << "No Parent Menu!";
    }

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::hover
 * This slot is called when the mouse is hovering over this action.
 * It passes the mouse control to this action's button.
 */
void ToolbarWidgetAction::hover()
{
    actionButton->stealMouse();
}


/**
 * @brief ToolbarWidgetAction::setParentMenuFocus
 * This slot is called when this action's parent menu gains/loses focus.
 * It determines whether this action and its button should track the mouse or not.
 * @param hasFocus
 */
void ToolbarWidgetAction::setParentMenuFocus(bool hasFocus)
{
    parentMenuHasFocus = hasFocus;
    actionButton->setAcceptMouseEvents(parentMenuHasFocus);
}


/**
 * @brief ToolbarWidgetAction::actionButtonPressed
 * This slot is called when this action's button is pressed.
 * It lets the toolbar know that this action was pressed.
 */
void ToolbarWidgetAction::actionButtonPressed()
{
    emit toolbarAction_pressed();
}


/**
 * @brief ToolbarWidgetAction::actionButtonClicked
 * This slot is called when this action's button is clicked.
 * It sets its the button's checked state and triggers this action.
 */
void ToolbarWidgetAction::actionButtonClicked()
{
    actionButton->setButtonChecked();

    // this is the signal that is sent when this action is triggered
    emit trigger();
}


/**
 * @brief ToolbarWidgetAction::resetActionButton
 * This slot is called whenever this action's child menu is about to hide.
 * It makes sure that this action's button's checked state is reset.
 */
void ToolbarWidgetAction::resetActionButton()
{
    if (actionButton && actionButton->isButtonChecked()) {
        actionButton->setButtonChecked();
    }
}
