#include "searchmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Views/Search/searchdialog.h"
#include "../../Views/Search/searchpopup.h"
#include "../../Views/Search/gotopopup.h"
#include "../WindowManager/windowmanager.h"

#include <QApplication>

#include <QDebug>

SearchManager* SearchManager::manager_singleton = 0;


SearchManager::SearchManager(ViewController* controller){
    viewController = controller;
}

SearchManager::~SearchManager(){

}

bool SearchManager::construct_singleton(ViewController* controller){
    if (!manager_singleton){
        manager_singleton = new SearchManager(controller);
        return true;
    }
    return false;
}

void SearchManager::destruct_singleton(){

}

SearchManager* SearchManager::manager(){
    return manager_singleton;
}

void SearchManager::Search(QString query){
    
    auto search_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    auto search_results = viewController->getSearchResults(query);
    auto search_finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "ViewController Search took: " <<  search_finish - search_start << "MS. Returned: " << search_results.size();
    auto panel = getSearchDialog();
    panel->DisplaySearchResults(query, search_results);
    auto search_display = QDateTime::currentDateTime().toMSecsSinceEpoch();
    emit SearchComplete();
    qCritical() << "Panel rendering search results: " <<  search_display - search_finish << "MS. Returned: " << search_results.size();
}


void SearchManager::PopupSearch(){
    auto popup = getSearchPopup();
    if(popup){
        auto search_suggestions = viewController->_getSearchSuggestions();
        popup->updateSearchSuggestions(search_suggestions);
        WindowManager::MoveWidget(popup);
        popup->show();
        popup->takeFocus();
    }
}
void SearchManager::PopupGoto(){
    auto popup = getGotoPopup();
    if(popup){
        popup->updateIDs(viewController->GetIDs());
        WindowManager::MoveWidget(popup);
        popup->show();
        popup->takeFocus();
    }
}

SearchPopup* SearchManager::getSearchPopup(){
    if(!search_popup){
        search_popup = new SearchPopup();
    }
    return search_popup;
}
GotoPopup* SearchManager::getGotoPopup(){
    if(!goto_popup){
        goto_popup = new GotoPopup();
    }
    return goto_popup;
}

SearchDialog* SearchManager::getSearchDialog(){
    if(!search_dialog){
        search_dialog = new SearchDialog(0);
        connect(viewController, &ViewController::vc_viewItemDestructing, search_dialog, &SearchDialog::viewItemDestructed);

        connect(search_dialog, &SearchDialog::CenterOn, viewController, &ViewController::centerOnID);
        connect(search_dialog, &SearchDialog::Popup, viewController, &ViewController::popupItem);
        connect(search_dialog, &SearchDialog::HighlightEntity, viewController, &ViewController::vc_highlightItem);

        //connect(search_dialog, &SearchDialog::itemHoverEnter, viewController->getToolbarController(), &ToolbarController::actionHoverEnter);
        //connect(search_dialog, &SearchDialog::itemHoverLeave, viewController->getToolbarController(), &ToolbarController::actionHoverLeave);

        //TODO:
        connect(search_dialog, &SearchDialog::SearchPopup, this, &SearchManager::PopupSearch);
        connect(search_dialog, &SearchDialog::SearchQuery, this, &SearchManager::Search);
    }
    return search_dialog;
}
