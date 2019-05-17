#include "searchmanager.h"
#include "../ViewController/viewcontroller.h"
#include "../../Views/Search/searchdialog.h"
#include "../../Views/Search/searchpopup.h"
#include "../../Views/Search/gotopopup.h"
#include "../WindowManager/windowmanager.h"

#include <QApplication>
#include <QDebug>

SearchManager* SearchManager::manager_singleton = 0;


/**
 * @brief SearchManager::SearchManager
 * @param controller
 */
SearchManager::SearchManager(ViewController* controller)
{
    viewController = controller;
}


/**
 * @brief SearchManager::construct_singleton
 * @param controller
 * @return
 */
bool SearchManager::construct_singleton(ViewController* controller)
{
    if (!manager_singleton) {
        manager_singleton = new SearchManager(controller);
        return true;
    }
    return false;
}


/**
 * @brief SearchManager::manager
 * @return
 */
SearchManager* SearchManager::manager()
{
    return manager_singleton;
}


/**
 * @brief SearchManager::Search
 * @param query
 */
void SearchManager::Search(const QString &query)
{
    auto search_results = viewController->getSearchResults(query);
    getSearchDialog()->DisplaySearchResults(query, search_results);
    emit SearchComplete();
}


/**
 * @brief SearchManager::PopupSearch
 */
void SearchManager::PopupSearch()
{
    auto popup = getSearchPopup();
    if (popup) {
        auto search_suggestions = viewController->_getSearchSuggestions();
        popup->updateSearchSuggestions(search_suggestions);
        WindowManager::MoveWidget(popup);
        popup->show();
        popup->takeFocus();
    }
}

/**
 * @brief SearchManager::PopupGoto
 */
void SearchManager::PopupGoto()
{
    auto popup = getGotoPopup();
    if (popup) {
        popup->updateIDs(viewController->GetIDs());
        WindowManager::MoveWidget(popup);
        popup->show();
        popup->takeFocus();
    }
}


/**
 * @brief SearchManager::getSearchPopup
 * @return
 */
SearchPopup* SearchManager::getSearchPopup()
{
    if (!search_popup) {
        search_popup = new SearchPopup();
    }
    return search_popup;
}


/**
 * @brief SearchManager::getGotoPopup
 * @return
 */
GotoPopup* SearchManager::getGotoPopup()
{
    if (!goto_popup) {
        goto_popup = new GotoPopup();
    }
    return goto_popup;
}


/**
 * @brief SearchManager::getSearchDialog
 * @return
 */
SearchDialog* SearchManager::getSearchDialog()
{
    if (!search_dialog) {
        search_dialog = new SearchDialog();

        connect(this, &SearchManager::ItemSelectionChanged, search_dialog, &SearchDialog::viewItemSelected);
        connect(viewController, &ViewController::vc_viewItemDestructing, search_dialog, &SearchDialog::viewItemDestructed);

        connect(search_dialog, &SearchDialog::CenterOn, viewController, &ViewController::centerOnID);
        connect(search_dialog, &SearchDialog::Popup, viewController, &ViewController::popupItem);
        connect(search_dialog, &SearchDialog::FlashEntity, [=] (int ID) {
            viewController->HighlightItems({ID});
        });

        //TODO:
        connect(search_dialog, &SearchDialog::SearchPopup, this, &SearchManager::PopupSearch);
        connect(search_dialog, &SearchDialog::SearchQuery, this, &SearchManager::Search);
    }

    return search_dialog;
}
