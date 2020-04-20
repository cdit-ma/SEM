#include "searchpopup.h"
#include "../../theme.h"
#include "../../Controllers/SearchManager/searchmanager.h"

#include <QStyledItemDelegate>
#include <QScrollBar>

/**
 * @brief SearchPopup::SearchPopup
 */
SearchPopup::SearchPopup()
        : PopupWidget(PopupWidget::TYPE::POPUP, nullptr)
{
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &SearchPopup::themeChanged);
    themeChanged();

    connect(search_bar, &QLineEdit::returnPressed, search_action, &QAction::trigger);
    connect(search_action, &QAction::triggered, this, &SearchPopup::SearchRequested);
    hide();
}


/**
 * @brief SearchPopup::takeFocus
 */
void SearchPopup::takeFocus()
{
    search_bar->setFocus();
    search_bar->selectAll();
}


/**
 * @brief SearchPopup::themeChanged
 */
void SearchPopup::themeChanged()
{
    auto theme = Theme::theme();
    toolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolBar{ spacing: 5px; }");
    toolbar->setIconSize(theme->getLargeIconSize() * 0.75);

    search_action->setIcon(theme->getIcon("Icons", "zoom"));
    search_bar->setStyleSheet(theme->getLineEditStyleSheet() + "QLineEdit{ padding: 3px; }");

    search_completer->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
    search_completer->popup()->verticalScrollBar()->setStyleSheet(theme->getScrollBarStyleSheet());
}


/**
 * @brief SearchPopup::SearchRequested
 */
void SearchPopup::SearchRequested()
{
    auto search_query = search_bar->text();
    SearchManager::manager()->Search(search_query);
    hide();
}


/**
 * @brief SearchPopup::updateSearchSuggestions
 * @param suggestions
 */
void SearchPopup::updateSearchSuggestions(const QStringList& suggestions)
{
    search_model->setStringList(suggestions);
}


/**
 * @brief SearchPopup::setupLayout
 */
void SearchPopup::setupLayout()
{
    search_model = new QStringListModel(this);

    search_completer = new QCompleter(this);
    search_completer->setModel(search_model);
    search_completer->setFilterMode(Qt::MatchContains);
    search_completer->setCaseSensitivity(Qt::CaseInsensitive);
    search_completer->popup()->setItemDelegate(new QStyledItemDelegate(this));
    search_completer->popup()->setFont(QFont(font().family(), 10));

    search_bar = new QLineEdit(this);
    search_bar->setFont(QFont(font().family(), 13, QFont::ExtraLight));
    search_bar->setPlaceholderText("Search...");
    search_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    search_bar->setCompleter(search_completer);
    search_bar->setAttribute(Qt::WA_MacShowFocusRect, false);

    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedWidth(450);
    toolbar->addWidget(search_bar);

    search_action = toolbar->addAction("Submit Search");

    int buttonSize = search_bar->height() + 10;
    toolbar->widgetForAction(search_action)->setFixedSize(buttonSize, buttonSize);

    setWidget(toolbar);
}
