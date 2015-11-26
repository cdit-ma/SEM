#include "aspecttogglewidget.h"
#include "../medeawindow.h"

#include "../enumerations.h"

#define HEIGHT_OFFSET 1.4
#define SHADOW_OFFSET 3

#define MIN_RGBA_VALUE 0
#define MAX_RGBA_VALUE 255

#define DEFAULT -1
#define CLICKED 0
#define DOUBLECLICKED 1
#define MIDDLECLICKED 2


/**
 * @brief AspectToggleWidget::AspectToggleWidget
 * @param text
 * @param size
 * @param parent
 */
AspectToggleWidget::AspectToggleWidget(VIEW_ASPECT aspect, double size, MedeaWindow* parentWindow) :
    QWidget(parentWindow)
{
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);

    viewAspect = aspect;
    aspectText = GET_ASPECT_NAME(aspect);

    CHECKED = false;
    ENABLED = true;
    STATE = DEFAULT;

    setupColor();
    setupLayout(size);

    connect(this, SIGNAL(aspectToggled(VIEW_ASPECT,bool)), parentWindow, SIGNAL(window_toggleAspect(VIEW_ASPECT,bool)));
    connect(this, SIGNAL(aspectToggle_doubleClicked(VIEW_ASPECT)), parentWindow, SIGNAL(window_centerAspect(VIEW_ASPECT)));
    connect(this, SIGNAL(aspectToggle_middleClicked(VIEW_ASPECT)), parentWindow, SIGNAL(window_aspectMiddleClicked(VIEW_ASPECT)));
    connect(parentWindow, SIGNAL(window_aspectMiddleClicked(VIEW_ASPECT)), this, SLOT(aspectMiddleClicked(VIEW_ASPECT)));
    connect(parentWindow, SIGNAL(window_SetViewVisible(bool)), this, SLOT(enableToggleButton(bool)));
}


/**
 * @brief AspectToggleWidget::getText
 * This returns the label/name of the aspect this toggle is linked to.
 * @return
 */
QString AspectToggleWidget::getText()
{
    return aspectText;
}


/**
 * @brief AspectToggleWidget::getToggleGridPos
 * This returns the grid position of this aspect toggle in MEDEA window.
 * @return
 */
QPoint AspectToggleWidget::getToggleGridPos()
{
    switch (GET_ASPECT_POS(viewAspect)) {
    case VAP_TOPLEFT:
        return QPoint(1,1);
    case VAP_TOPRIGHT:
        return QPoint(1,2);
    case VAP_BOTTOMLEFT:
        return QPoint(2,1);
    case VAP_BOTTOMRIGHT:
        return QPoint(2,2);
    default:
        return QPoint(-1,-1);
    }
}


/**
 * @brief AspectToggleWidget::getAspect
 * @return
 */
VIEW_ASPECT AspectToggleWidget::getAspect()
{
    return viewAspect;
}


/**
 * @brief AspectToggleWidget::click
 * This is triggered when this aspect toggle is clicked.
 * It checks what kind of click the event was and sets the CHECKED state
 * depeding on that. It sends a signal to the window that this was clicked.
 * @param checked - set CHECKED to this if state allows it
 * @param state - CLICKED, DOUBLECLICKED, MIDDLECLICKED
 */
void AspectToggleWidget::click(bool checked, int state)
{
    switch (state) {
    case CLICKED:
        //qDebug() << "CLICKED";
        CHECKED = checked;
        break;
    case DOUBLECLICKED:
        //qDebug() << "DOUBLECLICKED";
        CHECKED = true;
        stateChanged();
        emit aspectToggle_doubleClicked(getAspect());
        return;
    case MIDDLECLICKED:
        CHECKED = true;
        emit aspectToggle_middleClicked(getAspect());
        break;
    default:
        break;
    }
    stateChanged();
}


/**
 * @brief AspectToggleWidget::isClicked
 * This returns the CHECKED state of this toggle.
 * @return
 */
bool AspectToggleWidget::isClicked()
{
    return CHECKED;
}


/**
 * @brief AspectToggleWidget::setClicked
 * This sets the CHECKED state of this toggle and updates its stylesheet.
 * @param checked - new CHECKED state
 */
void AspectToggleWidget::setClicked(bool checked)
{
    if (CHECKED != checked) {
        CHECKED = checked;
        stateChanged();
    }
}


/**
 * @brief AspectToggleWidget::mouseReleaseEvent
 * This triggers the click event when the mouse is released within the bounds
 * of this toggle. It sends the state of the click, and then resets it.
 * @param event
 */
void AspectToggleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button()) {
    case Qt::LeftButton:
        if (STATE !=  DOUBLECLICKED) {
            STATE = CLICKED;
        }
        break;
    case Qt::MiddleButton:
        STATE = MIDDLECLICKED;
        break;
    default:
        return;
    }

    click(!CHECKED, STATE);

    // reset the state
    STATE = DEFAULT;
}


/**
 * @brief AspectToggleWidget::mouseDoubleClickEvent
 * This updates the current click state of this toggle to DOUBLECLICKED.
 * @param event
 */
void AspectToggleWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        STATE = DOUBLECLICKED;
    }
}


/**
 * @brief AspectToggleWidget::aspectMiddleClicked
 * This is called when an aspect toggle has been double-clicked.
 * If that toggle is not this, then un-click this toggle.
 * @param aspect - aspect that was doucble-clicked
 */
void AspectToggleWidget::aspectMiddleClicked(VIEW_ASPECT aspect)
{
    if (viewAspect != aspect) {
        setClicked(false);
    }
}


/**
 * @brief AspectToggleWidget::highlightToggleButton
 * This slot is called by the view when there is more than one aspects displayed and
 * the displayed aspects has been changed or the view has been scaled/panned.
 * @param aspect
 */
void AspectToggleWidget::highlightToggleButton(VIEW_ASPECT aspect)
{
    if (viewAspect == aspect) {
        aspectLabel->setStyleSheet("background-color: rgba(0,0,0,0);"
                                   //"font-size: 11.5px;"
                                   "font-weight: bold;"
                                   "color: white;");
    } else {
        aspectLabel->setStyleSheet("background-color: rgba(0,0,0,0);"
                                   //"font-size: 12px;"
                                   "color: black;");
    }
}


/**
 * @brief AspectToggleWidget::enableToggleButton
 * @param enable
 */
void AspectToggleWidget::enableToggleButton(bool enable)
{
    if (ENABLED != enable) {
        ENABLED = enable;
        updateStyleSheet();
    }
}


/**
 * @brief AspectToggleWidget::toColorStr
 * @param color
 * @param alpha
 * @return
 */
QString AspectToggleWidget::colorToString(QColor color, int alpha)
{
    if (alpha > MAX_RGBA_VALUE || alpha < MIN_RGBA_VALUE) {
        alpha = MAX_RGBA_VALUE;
    }

    QString colorStr = "rgba(";
    colorStr += QString::number(color.red()) + ",";
    colorStr += QString::number(color.green()) + ",";
    colorStr += QString::number(color.blue()) + ",";
    colorStr += QString::number(alpha) + ")";
    return colorStr;
}


/**
 * @brief AspectToggleWidget::adjustColorRGB
 * @param color
 * @param delta
 * @return
 */
QColor AspectToggleWidget::adjustColorRGB(QColor color, int delta)
{
    if (delta < 0) {
        color.setRed(qMax(color.red() + delta, MIN_RGBA_VALUE));
        color.setGreen(qMax(color.green() + delta, MIN_RGBA_VALUE));
        color.setBlue(qMax(color.blue() + delta, MIN_RGBA_VALUE));
    } else {
        color.setRed(qMin(color.red() + delta, MAX_RGBA_VALUE));
        color.setGreen(qMin(color.green() + delta, MAX_RGBA_VALUE));
        color.setBlue(qMin(color.blue() + delta, MAX_RGBA_VALUE));
    }
    return color;
}


/**
 * @brief AspectToggleWidget::setupColorMap
 * This sets up the colors used for the default and the gradient.
 */
void AspectToggleWidget::setupColor()
{
    QColor aspectColor = GET_ASPECT_COLOR(viewAspect);
    QColor darkerAspectColor = adjustColorRGB(aspectColor, -55); //-15);
    int checkedAlpha = 250;

    disabledColor = colorToString(Qt::darkGray);
    defaultColor = colorToString(darkerAspectColor);
    p1_Color = colorToString(adjustColorRGB(darkerAspectColor, 175), checkedAlpha);
    p2_Color = colorToString(adjustColorRGB(darkerAspectColor, 15), checkedAlpha);
}


/**
 * @brief AspectToggleWidget::setupLayout
 * This sets up thid toggle widgets layout.
 * @param size - fixed size of this widget
 */
void AspectToggleWidget::setupLayout(double widgetSize)
{
    setFixedSize(widgetSize, widgetSize / HEIGHT_OFFSET);
    setStyleSheet("border: none;");

    shadowFrame->setFixedSize(size().width(), size().height());
    shadowFrame->setStyleSheet("background-color: rgba(10,10,10,130); border-radius: 10px;");

    mainFrame->setFixedSize(size().width() - SHADOW_OFFSET, size().height() - SHADOW_OFFSET);
    updateStyleSheet();

    QFont font = this->font();
    font.setPointSizeF(9);

    aspectLabel = new QLabel(aspectText, this);
    aspectLabel->setFont(font);
    aspectLabel->setStyleSheet("background-color: rgba(0,0,0,0);");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(aspectLabel);
    layout->setAlignment(aspectLabel, Qt::AlignCenter);
    mainFrame->setLayout(layout);
}


/**
 * @brief AspectToggleWidget::stateChanged
 * This method updates the styleheet of this widget and sends a
 * signal to NodeView telling it that its checked state has changed.
 */
void AspectToggleWidget::stateChanged()
{
    updateStyleSheet();
    emit aspectToggled(viewAspect, CHECKED);
}


/**
 * @brief AspectToggleWidget::updateStyleSheet
 */
void AspectToggleWidget::updateStyleSheet()
{
    if (ENABLED) {
        if (CHECKED) {
            mainFrame->move(SHADOW_OFFSET, SHADOW_OFFSET);
            mainFrame->setStyleSheet("border-radius: 8px;"
                                     "background-color:"
                                     "qlineargradient(x1:0, y1:0, x2:0, y2:1.0,"
                                     "stop:0 " + p1_Color + ", stop:1.0 " + p2_Color + ");");
        } else {
            mainFrame->move(0, 0);
            mainFrame->setStyleSheet("border-radius: 8px; background-color:" + defaultColor + ";");
        }
    } else {
        mainFrame->setStyleSheet("border-radius: 8px; background-color:" + disabledColor + ";");
    }
}
