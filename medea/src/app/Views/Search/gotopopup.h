#ifndef GOTOPOPUP_H
#define GOTOPOPUP_H

#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>
#include <QToolBar>

#include "../../Widgets/Dialogs/popupwidget.h"

class GotoPopup : public PopupWidget
{
    Q_OBJECT
    
public:
    GotoPopup();
    
    void updateIDs(const QStringList& ids);
    void takeFocus();
    
private:
    void GotoRequested();

    void themeChanged();
    void setupLayout();

    QCompleter* search_completer = nullptr;
    QStringListModel* search_model = nullptr;
    
    QLineEdit* search_bar = nullptr;
    QToolBar* toolbar = nullptr;
    QAction* goto_action = nullptr;
};

#endif // GOTOPOPUP_H