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

    void updateSearchSuggestions(const QStringList& suggestions);
    void takeFocus();

private:
    void SearchRequested();
    void themeChanged();
    void setupLayout();

    QCompleter* search_completer = nullptr;
    QStringListModel* search_model = nullptr;
    QLineEdit* search_bar = nullptr;
    QToolBar* toolbar = nullptr;
    QAction* search_action = nullptr;
};

#endif // SEARCHPOPUP_H