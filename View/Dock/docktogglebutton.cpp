#include "docktogglebutton.h"
#include "dockscrollarea.h"

#include "../../medeawindow.h"

#include <QPainter>
#include <QPainterPath>
#include <QBrush>

#define DEFAULT 0
#define DISABLED 1
#define SELECTED 2


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

    selected = false;
    enabled = false;

    defaultPenWidth = 1;
    selectedPenWidth = 2;
    penWidth = defaultPenWidth;

    defaultPenColor = QColor(100,100,100);
    disabledPenColor = QColor(150,150,150);
    selectedPenColor = Qt::blue;
    penColor = defaultPenColor;

    defaultBrushColor = QColor(230, 230, 230);
    disabledBrushColor = QColor(180, 180, 180);
    selectedBrushColor = Qt::white;
    brushColor = defaultBrushColor;

    dockType = type;
    width = 40;
    height = 40;

    setFixedSize(width + 10, height + 10);

    QVBoxLayout* vLayout = new QVBoxLayout();
    QLabel* imageLabel = new QLabel(this);

    QSize pixmapSize = size() * 0.6;
    QSize labelSize = size();

    QPixmap pixmap;
    switch (type) {
    case PARTS_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Actions/Parts.png"));
        break;
    case DEFINITIONS_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Actions/Definition.png"));
        pixmapSize /= 1.5;
        break;
    case FUNCTIONS_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Actions/Function.png"));
        pixmapSize /= 1.25;
        break;
    case HARDWARE_DOCK:
        pixmap = QPixmap::fromImage(QImage(":/Items/HardwareNode.png"));
        break;
    default:
        break;
    }

    pixmap = pixmap.scaled(pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    imageLabel->setFixedSize(labelSize);
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    vLayout->setMargin(0);
    vLayout->addWidget(imageLabel);

    setLayout(vLayout);
    setStyleSheet("padding: 0px 5px 5px 0px;");

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
 * @brief DockToggleButton::paintEvent
 * Draw the button as an ellipse and change its color when it's selected.
 * The rect used for the path is moved to (1,1) so that the full pen width is painted.
 * @param e
 */
void DockToggleButton::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen pen(penColor);
    QBrush brush(brushColor);

    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // fill the ellipse
    QPainterPath path1;
    QRectF rect1(1, 1, width+1, height+1);
    path1.addEllipse(rect1);
    painter.fillPath(path1, brush);

    // draw the ellipse
    QRectF rect2(1, 1, width+2, height+2);
    QPainterPath path2;
    path2.addEllipse(rect2);
    painter.drawPath(path2);

    QWidget::paintEvent(e);
}


/**
 * @brief DockToggleButton::hideDock
 * When this button's groupbox was the last one displayed,
 * this method is called by the main window to make sure that
 * it is hidden before another groupbox is displayed.
 */
void DockToggleButton::hideDock()
{
    // if the groupbox is still visible, force press this button to hide it
    if (selected) {
        dockButtonPressed(getDockType());
    }
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
        connect(dock, SIGNAL(dock_forceOpenDock(DOCK_TYPE,QString)), parentWindow, SLOT(forceOpenDock(DOCK_TYPE,QString)));
    }
}


/**
 * @brief DockToggleButton::getWidth
 * Returns the width of this button.
 * @return width
 */
int DockToggleButton::getWidth()
{
    return width;
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
            setColor(SELECTED, true);
        } else {
            setColor(DEFAULT, true);
        }
        // show/hide the attached dock
        getDock()->setDockOpen(selected);
    }
}


/**
 * @brief DockToggleButton::setColor
 * This sets the colour of this toggle button depending on its current state.
 * Repaint the button only if a repaint is required.
 * @param state
 * @param needRepaint
 */
void DockToggleButton::setColor(int state, bool needRepaint)
{
    penWidth = defaultPenWidth;

    switch (state) {
    case DEFAULT:
        penColor = defaultPenColor;
        brushColor = defaultBrushColor;
        break;
    case DISABLED:
        penColor = disabledPenColor;
        brushColor = disabledBrushColor;
        break;
    case SELECTED:
        penWidth = selectedPenWidth;
        penColor = selectedPenColor;
        brushColor = selectedBrushColor;
        break;
    }

    if (needRepaint) {
        repaint();
    }
}


/**
 * @brief DockToggleButton::setEnabled
 * This enables/disables this dock toggle button, updating its selected and enabled state and its colour.
 * If this button was previously selected, it makes sure that the dock attched to it is closed.
 * @param enable
 * @param repaint
 */
void DockToggleButton::setEnabled(bool enable, bool repaint)
{
    if (enable) {
        if (selected) {
            setColor(SELECTED, repaint);
        } else {
            setColor(DEFAULT, repaint);
        }
    } else {
        hideDock();
        setSelected(false);
        setColor(DISABLED, repaint);
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
