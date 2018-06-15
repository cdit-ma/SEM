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
    void updateIDs(QStringList ids);
    void takeFocus();
private:
    void GotoRequested();

    void themeChanged();
    void setupLayout();

    QCompleter* search_completer = 0;
    QStringListModel* search_model = 0;
    QLineEdit* search_bar = 0;
    QToolBar* toolbar = 0;
    QAction* goto_action = 0;
};

#endif //GOTOPOPUP_H
