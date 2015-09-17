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
    parentMenu = parent;
    nodeItem = 0;
    actionButton = 0;
    kind = nodeKind;
    label = nodeKind;

    widgetMenu = 0;
    willHaveMenu = false;
    deletable = true;

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
    }
    // connect the new menu
    widgetMenu = menu;
    if (widgetMenu) {
        connect(this, SIGNAL(triggered()), widgetMenu, SLOT(execMenu()));
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
QPushButton *ToolbarWidgetAction::getButton()
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
QString ToolbarWidgetAction::getNodeItemID()
{
   if (nodeItem) {
       return nodeItem->getID();
   }
   return "";
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
QWidget* ToolbarWidgetAction::createWidget(QWidget *parent)
{
    actionButton = new ToolbarWidgetButton(parent);
    actionButton->setMouseTracking(true);

    actionButton->setMinimumHeight(33);
    actionButton->setStyleSheet("QPushButton{"
                                "border: 0px;"
                                "margin: 0px;"
                                "padding: 0px;"
                                "background-color: rgba(0,0,0,0);"
                                "}");

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);

    QString actionKind = getActionKind();
    if (actionKind == "HardwareNode"){
        Node* node = nodeItem->getNode();

        if(node){
            if(node->getDataValue("localhost") == "true"){
                actionKind = "Localhost";
            }else{
                actionKind = node->getDataValue("os") + "_" + node->getDataValue("architecture");
            }
        }

    }
    QString alias = "Items";
    if(actionKind == "Info"){
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

    if (isEnabled()) {
        connect(this, SIGNAL(hovered()), this, SLOT(hover()));
        connect(actionButton, SIGNAL(pressed()), this, SLOT(actionButtonPressed()));
        connect(actionButton, SIGNAL(clicked()), this, SLOT(actionButtonClicked()));
        connect(actionButton, SIGNAL(mouseEntered()), this, SLOT(hover()));
        connect(actionButton, SIGNAL(mouseExited()), this, SLOT(endHover()));
    }

    return actionButton;
}


/**
 * @brief ToolbarWidgetAction::hover
 * This method passes the hover on this item to the actionButton.
 */
void ToolbarWidgetAction::hover()
{
    QPalette pal = actionButton->palette();
    pal.setColor(QPalette::Button, QColor(10,10,10,50));
    actionButton->setAutoFillBackground(true);
    actionButton->setPalette(pal);
    actionButton->update();

    /*
    if (widgetMenu && !widgetMenu->isVisible()) {
        widgetMenu->execMenu();
    }
    */

    actionButton->grabMouse();
    actionButton->releaseMouse();
}

/**
 * @brief ToolbarWidgetAction::endHover
 * This slot is called when the mouse leaves a hover
 */
void ToolbarWidgetAction::endHover()
{
    if(!actionButton->getCheck()) {
        QPalette pal = actionButton->palette();
        pal.setColor(QPalette::Button, QColor(0,0,0,0));
        actionButton->setAutoFillBackground(true);
        actionButton->setPalette(pal);
        actionButton->update();
    }

    /*
    if (actionButton->getCheck() && (widgetMenu && widgetMenu->isVisible())) {
        widgetMenu->close();
    }
    */
}


/**
 * @brief ToolbarWidgetAction::actionButtonPressed
 */
void ToolbarWidgetAction::actionButtonPressed()
{
    emit toolbarAction_pressed();
}


/**
 * @brief ToolbarWidgetAction::actionButtonClicked
 * When the actionButton is clicked, this action is triggered.
 */
void ToolbarWidgetAction::actionButtonClicked()
{
    QPalette pal = actionButton->palette();
    pal.setColor(QPalette::Button, QColor(10,10,10,50));
    actionButton->setAutoFillBackground(true);
    actionButton->setPalette(pal);
    actionButton->update();

    actionButton->setCheck(true);
    emit trigger();
}


/**
 * @brief ToolbarWidgetAction::menuOpened
 */
void ToolbarWidgetAction::menuOpened()
{
   actionButton->setCheck(true);
}


/**
 * @brief ToolbarWidgetAction::menuClosed
 */
void ToolbarWidgetAction::menuClosed()
{
    actionButton->setCheck(false);
}
