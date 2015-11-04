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

#define THEME_LIGHT 0
#define THEME_DARK_NEUTRAL 1
#define THEME_DARK_COLOURED 2


/**
 * @brief AspectToggleWidget::AspectToggleWidget
 * @param text
 * @param size
 * @param parent
 */
AspectToggleWidget::AspectToggleWidget(VIEW_ASPECT aspect, double size, MedeaWindow* parent) :
    QWidget(parent)
{
    shadowFrame = new QFrame(this);
    mainFrame = new QFrame(this);

    viewAspect = aspect;
    aspectText = GET_ASPECT_NAME(aspect);

    CHECKED = false;
    STATE = DEFAULT;
    THEME = THEME_LIGHT;
    //THEME = THEME_DARK_COLOURED;
    //THEME = THEME_DARK_NEUTRAL;

    setupColor();
    setupLayout(size);

    connect(this, SIGNAL(aspectToggled(VIEW_ASPECT,bool)), parent, SIGNAL(window_toggleAspect(VIEW_ASPECT,bool)));
    connect(this, SIGNAL(aspectToggle_doubleClicked(VIEW_ASPECT)), parent, SIGNAL(window_aspectDoubleClicked(VIEW_ASPECT)));
    connect(this, SIGNAL(aspectToggle_middleClicked(VIEW_ASPECT)), parent, SIGNAL(window_centerAspect(VIEW_ASPECT)));
    connect(parent, SIGNAL(window_aspectDoubleClicked(VIEW_ASPECT)), this, SLOT(aspectDoubleClicked(VIEW_ASPECT)));
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
        CHECKED = checked;
        break;
    case DOUBLECLICKED:
        CHECKED = true;
        emit aspectToggle_doubleClicked(getAspect());
        break;
    case MIDDLECLICKED:
        CHECKED = true;
        stateChanged();
        emit aspectToggle_middleClicked(getAspect());
        return;
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
    CHECKED = checked;
    stateChanged();
}


/**
 * @brief AspectToggleWidget::mouseReleaseEvent
 * This triggers the click event when the mouse is released within the bounds
 * of this toggle. It sends the state of the click, and then resets it.
 * @param event
 */
void AspectToggleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (rect().contains(event->pos())) {
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
            break;
        }
        click(!CHECKED, STATE);
    }

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
 * @brief AspectToggleWidget::aspectDoubleClicked
 * This is called when an aspect toggle has been double-clicked.
 * If that toggle is not this, then un-click this toggle.
 * @param aspect - aspect that was doucble-clicked
 */
void AspectToggleWidget::aspectDoubleClicked(VIEW_ASPECT aspect)
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
    QColor darkerAspectColor = adjustColorRGB(aspectColor, -55);
    int checkedAlpha = 250;

    defaultColor = colorToString(darkerAspectColor);
    p1_Color = colorToString(adjustColorRGB(darkerAspectColor, 175), checkedAlpha);
    p2_Color = colorToString(adjustColorRGB(darkerAspectColor, 15), checkedAlpha);

    /*
    QString defaultAlpha = "255";
    QString checkedAlpha = "250";
    switch (THEME) {
    case THEME_DARK_COLOURED:
        if (aspectText == "Interfaces") {
            defaultColor = "rgba(24,148,184," + defaultAlpha + ")";
            p1_Color = "rgba(194,255,255," + checkedAlpha + ")";
            p2_Color = "rgba(0,128,164," + checkedAlpha + ")";
        } else if (aspectText == "Behaviour") {
            defaultColor = "rgba(155,155,155," + defaultAlpha + ")";
            p1_Color = "rgba(255,255,255," + checkedAlpha + ")";
            p2_Color = "rgba(135,135,135," + checkedAlpha + ")";
        } else if (aspectText == "Assemblies") {
            defaultColor = "rgba(90,90,90," + defaultAlpha + ")";
            p1_Color = "rgba(210,210,210," + checkedAlpha + ")";
            p2_Color = "rgba(70,70,70," + checkedAlpha + ")";
        } else if (aspectText == "Hardware") {
            defaultColor = "rgba(207,107,100," + defaultAlpha + ")";
            p1_Color = "rgba(255,207,200," + checkedAlpha + ")";
            p2_Color = "rgba(187,87,80," + checkedAlpha + ")";
        }
        break;
    case THEME_DARK_NEUTRAL:
        if (aspectText == "Interfaces") {
            defaultColor = "rgba(134,161,170," + defaultAlpha + ")";
            p1_Color = "rgba(184,191,200," + checkedAlpha + ")";
            p2_Color = "rgba(114,141,150," + checkedAlpha + ")";
        } else if (aspectText == "Behaviour") {
            defaultColor = "rgba(164,176,172," + defaultAlpha + ")";
            p1_Color = "rgba(214,206,202," + checkedAlpha + ")";
            p2_Color = "rgba(144,156,152," + checkedAlpha + ")";
        } else if (aspectText == "Assemblies") {
            defaultColor = "rgba(192,191,197," + defaultAlpha + ")";
            p1_Color = "rgba(232,231,237," + checkedAlpha + ")";
            p2_Color = "rgba(172,171,177," + checkedAlpha + ")";
        } else if (aspectText == "Hardware") {
            defaultColor = "rgba(239,238,233," + defaultAlpha + ")";
            p1_Color = "rgba(255,255,255," + checkedAlpha + ")";
            p2_Color = "rgba(219,218,213," + checkedAlpha + ")";
        }
        break;
    default:
        break;
    }
    */
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
    if (CHECKED) {
        mainFrame->move(SHADOW_OFFSET, SHADOW_OFFSET);
        mainFrame->setStyleSheet("border-radius: 8px;"
                                 "background-color:"
                                 "qlineargradient(x1:0, y1:0, x2:0, y2:1.0,"
                                 "stop:0 " + p1_Color + ", stop:1.0 " + p2_Color + ");");
        //"stop:0 " + p1_Color + ", stop:0.5 " + p3_Color + "," +
        //"stop:0.5 " + p4_Color + ", stop:1.0 " + p2_Color + ");");
    } else {
        mainFrame->move(0, 0);
        mainFrame->setStyleSheet("border-radius: 8px; background-color:" + defaultColor + ";");
    }\
}
