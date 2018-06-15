#include "gotopopup.h"
#include "../../theme.h"
#include "../../Controllers/SearchManager/searchmanager.h"
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QIntValidator>

GotoPopup::GotoPopup():PopupWidget(PopupWidget::TYPE::POPUP, 0) {
    setupLayout();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &GotoPopup::themeChanged);
    themeChanged();

    connect(search_bar, &QLineEdit::returnPressed, goto_action, &QAction::trigger);
    connect(goto_action, &QAction::triggered, this, &GotoPopup::GotoRequested);
    hide();
}


void GotoPopup::takeFocus(){
    search_bar->setFocus();
    search_bar->selectAll();
}

void GotoPopup::themeChanged(){
    auto theme = Theme::theme();

    toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    
    search_bar->setStyleSheet(theme->getLineEditStyleSheet());
    goto_action->setIcon(theme->getIcon("Icons", "crosshair"));
    search_completer->popup()->setStyleSheet(theme->getAbstractItemViewStyleSheet() % theme->getScrollBarStyleSheet() % "QAbstractItemView::item{ padding: 2px 0px; }");

    toolbar->setIconSize(theme->getIconSize());
}

void GotoPopup::GotoRequested(){
    bool success = false;
    auto id = search_bar->text().toInt(&success);
    if(success){
        SearchManager::manager()->GotoID(id);
    }
    hide();
}

void GotoPopup::updateIDs(QStringList suggestions){
    search_model->setStringList(suggestions);
}

void GotoPopup::setupLayout(){
    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedWidth(300);
    toolbar->setStyleSheet("QToolBar{padding:2px;}");

    search_model = new QStringListModel(this);

    auto validator = new QIntValidator(this);
    
    
    search_completer = new QCompleter(this);
    search_completer->setModel(search_model);
    search_completer->setFilterMode(Qt::MatchContains);
    search_completer->setCaseSensitivity(Qt::CaseInsensitive);
    search_completer->popup()->setItemDelegate(new QStyledItemDelegate(this));
    search_completer->popup()->setFont(QFont(font().family(), 10));

    
    search_bar = new QLineEdit(this);
    search_bar->setFont(QFont(font().family(), 13));
    search_bar->setPlaceholderText("Goto ID");
    search_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    search_bar->setCompleter(search_completer);
    search_bar->setValidator(validator);
    search_bar->setAttribute(Qt::WA_MacShowFocusRect, false);
    
    toolbar->addWidget(search_bar);
    goto_action = toolbar->addAction("Goto");
    setWidget(toolbar);
}