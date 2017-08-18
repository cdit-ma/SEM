#include "searchpopup.h"
#include "../../theme.h"
#include "../../Controllers/SearchManager/searchmanager.h"

#include <QStyledItemDelegate>

SearchPopup::SearchPopup():PopupWidget(PopupWidget::TYPE::TOOL, 0) {
    setupLayout();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &SearchPopup::themeChanged);
    themeChanged();

    connect(search_bar, &QLineEdit::returnPressed, search_action, &QAction::trigger);
    connect(search_action, &QAction::triggered, this, &SearchPopup::SearchRequested);
}

void SearchPopup::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet("QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + "; }");

    toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    search_bar->setStyleSheet(theme->getLineEditStyleSheet());
    search_action->setIcon(theme->getIcon("Icons", "zoom"));
    search_completer->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");
}

void SearchPopup::SearchRequested(){
    auto search_query = search_bar->text();
    SearchManager::manager()->Search(search_query);
    hide();
}

void SearchPopup::updateSearchSuggestions(QStringList suggestions){
    search_model->setStringList(suggestions);
}

void SearchPopup::setupLayout(){
    toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(24,24));
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedWidth(300);

    search_model = new QStringListModel(this);
    
    search_completer = new QCompleter(this);
    search_completer->setModel(search_model);
    search_completer->setFilterMode(Qt::MatchContains);
    search_completer->setCaseSensitivity(Qt::CaseInsensitive);
    search_completer->popup()->setItemDelegate(new QStyledItemDelegate(this));
    search_completer->popup()->setFont(QFont(font().family(), 10));

    search_action = new QAction(this);
    search_action->setToolTip("Submit Search");

    
    search_bar = new QLineEdit(this);
    search_bar->setFont(QFont(font().family(), 13));
    search_bar->setPlaceholderText("Search MEDEA");
    search_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    search_bar->setCompleter(search_completer);

    toolbar->addWidget(search_bar);
    toolbar->addAction(search_action);
    setWidget(toolbar);
}