#include "filterwidget.h"
#include "../theme.h"

#include <QHBoxLayout>
#include <QDebug>


/**
 * @brief FilterWidget::FilterWidget
 */
FilterWidget::FilterWidget()
    : QWidget()
{
    setupLayout();
    
    connect(line_edit, &QLineEdit::textChanged, this, &FilterWidget::filterChanged);
    
    setFocusProxy(line_edit);
    setFocusPolicy(Qt::StrongFocus);
    line_edit->installEventFilter(this);

    setToolTip("Filter the Menu");

    connect(Theme::theme(), &Theme::theme_Changed, this, &FilterWidget::themeChanged);
    themeChanged();
}


/**
 * @brief FilterWidget::themeChanged
 */
void FilterWidget::themeChanged()
{
    auto theme = Theme::theme();
    line_edit->setStyleSheet(theme->getLineEditStyleSheet()  + "QLineEdit{border-radius: " + theme->getSharpCornerRadius() + ";}");


    // TODO - Right now, the palette and stylesheet for the placeholder text only get set once; why???
    auto line_edit_palette = line_edit->palette();
    line_edit_palette.setColor(QPalette::Inactive, QPalette::PlaceholderText, theme->getTextColor());
    line_edit_palette.setColor(QPalette::Active, QPalette::PlaceholderText, theme->getTextColor());
    line_edit->setPalette(line_edit_palette);
    line_edit->update();
}


/**
 * @brief FilterWidget::setupLayout
 */
void FilterWidget::setupLayout()
{
    auto layout = new QHBoxLayout(this);
    layout->setMargin(3);

    line_edit = new QLineEdit(this);
    line_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    line_edit->setAttribute(Qt::WA_MacShowFocusRect, false);
    line_edit->setPlaceholderText("Filter Entities...");

    line_edit->setFocusPolicy(Qt::StrongFocus);
    line_edit->setFocus();

    layout->addWidget(line_edit);
}


/**
 * @brief FilterWidget::eventFilter
 * @param object
 * @param event
 * @return
 */
bool FilterWidget::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::FocusIn) {
        if (object == line_edit) {
            QMetaObject::invokeMethod(line_edit, "selectAll", Qt::QueuedConnection);
        }
   }
   return false;
}
