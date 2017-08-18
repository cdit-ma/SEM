#ifndef SEARCHPOPUP_H
#define SEARCHPOPUP_H

#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>
#include <QToolBar>


#include "../../Widgets/Dialogs/popupwidget.h"

class SearchPopup : public PopupWidget
{
    Q_OBJECT
public:
    explicit SearchPopup();
    void updateSearchSuggestions(QStringList suggestions);
private:
    void SearchRequested();
    void themeChanged();
    void setupLayout();

    QCompleter* search_completer = 0;
    QStringListModel* search_model = 0;
    QLineEdit* search_bar = 0;
    QToolBar* toolbar = 0;
    QAction* search_action = 0;
};

#endif //SEARCHPOPUP_H
