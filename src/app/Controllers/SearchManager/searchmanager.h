#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include "../ActionController/actioncontroller.h"
#include <QObject>

class ViewController;
class SearchPopup;
class SearchDialog;
class GotoPopup;

class SearchManager : public QObject
{
    Q_OBJECT
    friend class ViewController;

protected:
    static bool construct_singleton(ViewController* controller);

signals:
    void SearchComplete();
    void GotoID(int id);
    void ItemSelectionChanged(ViewItem* item, bool selected);

public:
    static SearchManager* manager();
    
    SearchPopup* getSearchPopup();
    GotoPopup* getGotoPopup();
    SearchDialog* getSearchDialog();
    
    void Search(const QString& search_query);
    void PopupSearch();
    void PopupGoto();

private:
    explicit SearchManager(ViewController* controller);

    static SearchManager* manager_singleton;
    
    SearchPopup* search_popup = nullptr;
    GotoPopup* goto_popup = nullptr;
    SearchDialog* search_dialog = nullptr;
    ViewController* viewController = nullptr;
};

#endif // NOTIFICATIONMANAGER_H
