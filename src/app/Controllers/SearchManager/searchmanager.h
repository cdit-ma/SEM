#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include "../ActionController/actioncontroller.h"
#include <QObject>

class ViewController;
class SearchPopup;
class SearchDialog;

class SearchManager : public QObject
{
    Q_OBJECT
    friend class ViewController;
private:
    SearchManager(ViewController* controller);
    ~SearchManager();
protected:
    static bool construct_singleton(ViewController* controller);
    static void destruct_singleton();
signals:
    void SearchComplete();
public:
    static SearchManager* manager();
    
    SearchPopup* getSearchPopup();
    SearchDialog* getSearchDialog();
    
    void Search(QString search_query);
    void PopupSearch();
private:
    static SearchManager* manager_singleton;
    SearchPopup* search_popup = 0;
    SearchDialog* search_dialog = 0;
    ViewController* viewController = 0;
};

#endif // NOTIFICATIONMANAGER_H
