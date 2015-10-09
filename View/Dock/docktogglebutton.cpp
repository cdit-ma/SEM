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
 * @param label
 * @param parent
 */
DockToggleButton::DockToggleButton(QString label, MedeaWindow *window, QWidget *parent) :
    QPushButton(parent)
{
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

    kind = label;
    width = 40;
    height = 40;

    setText(label);
    setFixedSize(width + 10, height + 10);

    QVBoxLayout* vLayout = new QVBoxLayout();
    QLabel* imageLabel = new QLabel(this);

    QSize pixmapSize = size() * 0.6;
    QSize labelSize = size();

    QPixmap pixmap;
    if (label == "P") {
        pixmap = QPixmap::fromImage(QImage(":/Actions/Parts.png"));
    } else if (label == "H") {
        pixmap = QPixmap::fromImage(QImage(":/Items/HardwareNode.png"));
    } else if (label == "D") {
        pixmap = QPixmap::fromImage(QImage(":/Actions/Definition.png"));
        pixmapSize /= 1.5;
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
    connect(this, SIGNAL(pressed()), this, SLOT(on_buttonPressed()));
    connect(this, SIGNAL(dockButton_pressed(QString)), window, SLOT(dockButtonPressed(QString)));
}


/**
 * @brief DockToggleButton::~DockToggleButton
 */
DockToggleButton::~DockToggleButton()
{
    delete scrollArea;
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
 * @brief DockToggleButton::setContainer
 * @param area
 */
void DockToggleButton::setContainer(DockScrollArea *area)
{
    scrollArea = area;
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

    //painter.drawText(16, 25, text());

    QWidget::paintEvent(e);
}


/**
 * @brief DockToggleButton::hideContainer
 * When this button's groupbox was the last one displayed,
 * this method is called by the main window to make sure that
 * it is hidden before another groupbox is displayed.
 */
void DockToggleButton::hideContainer()
{
    // if the groupbox is still visible, force press this button to hide it
    if (selected) {
        emit pressed();
    }
}


/**
 * @brief DockToggleButton::getContainer
 * This method returns the dock container attached to this button.
 * @return scrollArea
 */
DockScrollArea* DockToggleButton::getContainer()
{
    return scrollArea;
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
 * @brief DockToggleButton::getKind
 * Returns the kind of this button.
 * @return - P, D, H
 */
QString DockToggleButton::getKind()
{
    return kind;
}


/**
 * @brief DockToggleButton::on_buttonPressed
 * When this button is pressed, its selected state is updated
 * and its text() is sent to the main window.
 */
void DockToggleButton::on_buttonPressed()
{
    if (selected) {
        selected = false;
        setColor(DEFAULT, true);
    } else {
        selected = true;
        setColor(SELECTED, true);
    }
    emit dockButton_pressed(this->text());
    emit dockButton_dockOpen(selected);
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
 */
void DockToggleButton::setEnabled(bool enable)
{
    if (enable) {
        if (selected) {
            setColor(SELECTED);
        } else {
            setColor(DEFAULT);
        }
    } else {
        hideContainer();
        setSelected(false);
        setColor(DISABLED);
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
