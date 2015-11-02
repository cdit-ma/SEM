#include "searchsuggestcompletion.h"

#include <QHeaderView>
#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

#define ITEM_PADDING 5

/**
 * @brief SearchSuggestCompletion::SearchSuggestCompletion
 * @param parent
 */
SearchSuggestCompletion::SearchSuggestCompletion(QLineEdit* parent) : QObject(parent), editor(parent)
{
    popup = new QTreeWidget;
    popup->setWindowFlags(Qt::Popup);
    popup->setFocusPolicy(Qt::NoFocus);
    popup->setFocusProxy(parent);
    popup->setMouseTracking(true);

    popup->setColumnCount(1);
    popup->setUniformRowHeights(true);
    popup->setRootIsDecorated(false);
    popup->setEditTriggers(QTreeWidget::NoEditTriggers);
    popup->setSelectionBehavior(QTreeWidget::SelectRows);
    popup->setFrameStyle(QFrame::Box | QFrame::Plain);
    popup->header()->hide();

    popup->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    popup->installEventFilter(this);

    QFont guiFont = QFont("Verdana");
    guiFont.setPointSizeF(8.5);
    popup->setFont(guiFont);

    connect(popup, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(doneCompletion()));
}


/**
 * @brief SearchSuggestCompletion::~SearchSuggestCompletion
 */
SearchSuggestCompletion::~SearchSuggestCompletion()
{
    delete popup;
}


/**
 * @brief SearchSuggestCompletion::eventFilter
 * @param obj
 * @param ev
 * @return
 */
bool SearchSuggestCompletion::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj != popup) {
        return false;
    }

    if (ev->type() == QEvent::MouseButtonPress) {
        popup->hide();
        editor->setFocus();
        return true;
    }

    if (ev->type() == QEvent::KeyPress) {

        bool consumed = false;

        int key = reinterpret_cast<QKeyEvent*>(ev)->key();

        switch (key) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            doneCompletion();
            consumed = true;

        case Qt::Key_Escape:
            editor->setFocus();
            popup->hide();
            consumed = true;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            break;

        default:
            editor->setFocus();
            editor->event(ev);
            break;
        }

        return consumed;
    }

    return false;
}


/**
 * @brief SearchSuggestCompletion::setSize
 * @param width
 * @param height
 * @param sizeKind - 0 = fixed, 1 = min, 2 = max
 */
void SearchSuggestCompletion::setSize(qreal width, qreal height, int sizeKind)
{
   if (width <= 0) {
       width = popup->width();
   }
   if (height <= 0) {
       height = popup->height();
   }
   switch (sizeKind) {
   case 1:
       popup->setMinimumSize(width, height);
       break;
   case 2:
       popup->setMaximumSize(width, height);
       break;
   default:
       popup->setFixedSize(width, height);
   }
}


/**
 * @brief SearchSuggestCompletion::showCompletion
 * @param choices
 */
void SearchSuggestCompletion::showCompletion(const QStringList &choices)
{
    if (choices.isEmpty()) {
        popup->clear();
        popup->hide();
        return;
    }

    const QPalette &pal = editor->palette();
    QColor color = pal.color(QPalette::Disabled, QPalette::WindowText);

    QFontMetrics fm = popup->fontMetrics();
    int totalHeight = popup->contentsMargins().top() + popup->contentsMargins().bottom();
    int maxWidth = editor->width();

    popup->setUpdatesEnabled(false);
    popup->clear();

    for (int i = 0; i < choices.count(); ++i) {
        QTreeWidgetItem * item;
        item = new QTreeWidgetItem(popup);
        item->setText(0, choices[i]);
        item->setTextColor(0, color);
        totalHeight += popup->visualItemRect(item).height();
        maxWidth = qMax(maxWidth, fm.width(choices[i] + 'W'));
    }

    popup->setFixedSize(maxWidth, totalHeight + ITEM_PADDING);
    //popup->setCurrentItem(popup->topLevelItem(0));
    popup->setUpdatesEnabled(true);

    popup->move(editor->mapToGlobal(QPoint(0, editor->height())));
    popup->setFocus();
    popup->show();
}


/**
 * @brief SearchSuggestCompletion::doneCompletion
 */
void SearchSuggestCompletion::doneCompletion()
{
    popup->hide();
    editor->setFocus();

    QTreeWidgetItem* item = popup->currentItem();
    if (item) {
        editor->setText(item->text(0));
        QMetaObject::invokeMethod(editor, "returnPressed");
    }
}
