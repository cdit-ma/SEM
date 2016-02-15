#include "docktogglebutton.h"
#include "dockscrollarea.h"

#include "../../medeawindow.h"

#include <QPainter>
#include <QPainterPath>
#include <QBrush>


//#define BUTTON_WIDTH 40
#define BUTTON_WIDTH 65
#define BUTTON_HEIGHT 42

#define DEFAULT 0
#define SELECTED 1


/**
 * @brief DockToggleButton::DockToggleButton
 * @param type
 * @param window
 * @param parent
 */
DockToggleButton::DockToggleButton(DOCK_TYPE type, MedeaWindow *window, QWidget *parent) :
    QPushButton(parent)
{
    dock = 0;
    parentWindow = window;
    dockType = type;

    selected = false;
    enabled = false;

    QSize buttonSize(BUTTON_WIDTH + 4, BUTTON_HEIGHT + 4);
    QSize pixmapSize = buttonSize * 0.6;
    QPixmap pixmap;

    switch (type) {
    case PARTS_DOCK:
        //pixmap = QPixmap::fromImage(QImage(":/Actions/Backward.png"));
        pixmap = QPixmap::fromImage(QImage(":/Actions/Parts.png"));
        setToolTip("Parts Dock");
        break;
    case DEFINITIONS_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Actions/Definition.png"));
        pixmapSize /= 1.5;
        setToolTip("Definitions Dock");
        break;
    case FUNCTIONS_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Actions/Function.png"));
        pixmapSize /= 1.25;
        setToolTip("Functions Dock");
        break;
    case HARDWARE_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Items/HardwareNode.png"));
        setToolTip("Hardware Dock");
        break;
    default:
        qWarning() << "DockToggleButton::DockToggleButton - Unknown dock type.";
        break;
    }

    fixedStyleSheet = "QPushButton:disabled {"
                      //"border: 1px solid rgb(140,140,140);"
                      "background: rgb(150,150,150);"
                      "}"
                      "QToolTip{ background: white; }";

    setFixedSize(buttonSize);
    setIconSize(pixmapSize);
    setIcon(QIcon(pixmap));
    updateStyleSheet(DEFAULT);

    // make connections
    connect(this, SIGNAL(pressed()), window, SLOT(dockButtonPressed()));
    connect(window, SIGNAL(window_dockButtonPressed(DOCK_TYPE)), this, SLOT(dockButtonPressed(DOCK_TYPE)));
}


/**
 * @brief DockToggleButton::~DockToggleButton
 */
DockToggleButton::~DockToggleButton()
{
    delete dock;
}


/**
 * @brief DockToggleButton::getSelected
 * Returns this button's selected state.
 * @return
 */
bool DockToggleButton::isSelected()
{
    return selected;
}


/**
 * @brief DockToggleButton::setSelected
 * Sets this button's selected state to b.
 * @param b
 */
void DockToggleButton::setSelected(bool b)
{
    selected = b;
}


/**
 * @brief DockToggleButton::hideDock
 * When this button's groupbox was the last one displayed,
 * this method is called by the main window to make sure that
 * it is hidden before another groupbox is displayed.
 */
void DockToggleButton::hideDock()
{
    if (dock) {
        dock->setDockOpen(false);
    }
    setSelected(false);
}


/**
 * @brief DockToggleButton::getDock
 * This method returns the dock attached to this button.
 * @return scrollArea
 */
DockScrollArea* DockToggleButton::getDock()
{
    return dock;
}


/**
 * @brief DockToggleButton::setDock
 * @param dock
 */
void DockToggleButton::setDock(DockScrollArea* dock)
{
    this->dock = dock;

    // connect the dock to the parent window
    if (parentWindow && dock) {
        connect(parentWindow, SIGNAL(window_clearDocks()), dock, SLOT(clear()));
        connect(parentWindow, SIGNAL(window_clearDocksSelection()), dock, SLOT(clearSelected()));
    }
}


/**
 * @brief DockToggleButton::getWidth
 * Returns the width of this button.
 * @return width
 */
int DockToggleButton::getWidth()
{
    return BUTTON_WIDTH;
}


/**
 * @brief DockToggleButton::getHeight
 * Returns the height of this button.
 * @return
 */
int DockToggleButton::getHeight()
{
   return BUTTON_HEIGHT;
}


/**
 * @brief DockToggleButton::getDockType
 * @return
 */
DOCK_TYPE DockToggleButton::getDockType()
{
    return dockType;
}


/**
 * @brief DockToggleButton::getParentWindow
 * @return
 */
MedeaWindow *DockToggleButton::getParentWindow()
{
    return parentWindow;
}


/**
 * @brief DockToggleButton::dockButtonPressed
 * @param type
 */
void DockToggleButton::dockButtonPressed(DOCK_TYPE type)
{
    bool needUpdate = true;

    // update the selected state
    if (type == getDockType()) {
        selected = !selected;
    } else {
        if (selected) {
            selected = false;
        } else {
            needUpdate = false;
        }
    }

    // update the button's color if the state is changed
    if (needUpdate) {
        if (selected) {
            updateStyleSheet(SELECTED);
        } else {
            updateStyleSheet(DEFAULT);
        }
    }

    // show/hide the attached dock
    getDock()->setDockOpen(selected);
}


/**
 * @brief DockToggleButton::updateStyleSheet
 * @param state
 */
void DockToggleButton::updateStyleSheet(int state)
{
    QString borderStyleSheet;
    QString backgroundStyleSheet;

    switch (state) {
    case DEFAULT:
        borderStyleSheet = "border: 1px solid rgb(125,125,125);";
        backgroundStyleSheet = "background: rgb(235,235,235);";
        break;
    case SELECTED:
        borderStyleSheet = "border: 2px solid rgb(50,50,250);";
        backgroundStyleSheet = "background: rgb(250,250,250);";
        break;
    default:
        return;
    }

    setStyleSheet("QPushButton {"
                  "padding: 0px;"
                  + borderStyleSheet
                  + backgroundStyleSheet +
                  "}" + fixedStyleSheet);
}


/**
 * @brief DockToggleButton::setEnabled
 * This enables/disables this dock toggle button, updating its selected and enabled state and its colour.
 * If this button was previously selected, it makes sure that the dock attached to it is closed.
 * @param enable
 */
void DockToggleButton::setEnabled(bool enable)
{
    if (enable) {
        if (selected) {
            updateStyleSheet(SELECTED);
        } else {
            updateStyleSheet(DEFAULT);
        }
    } else {
        hideDock();
        setSelected(false);
    }

	enabled = enable;
    QPushButton::setEnabled(enabled);
}


/**
 * @brief DockToggleButton::isEnabled
 * QT's isEnabled function is as "reliable" as its isVisible function.
 * @return
 */
bool DockToggleButton::isEnabled()
{
    return enabled;
}
