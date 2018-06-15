#ifndef POPUPDATAEDITOR_H
#define POPUPDATAEDITOR_H

#include <QToolBar>
#include "../../Widgets/Dialogs/popupwidget.h"
#include "../../Widgets/dataeditwidget.h"
#include "../../Controllers/ViewController/viewitem.h"

class PopupDataEditor : public PopupWidget
{
    Q_OBJECT
public:
    PopupDataEditor();
    void edit(ViewItem* view_item, QString key_name);
    void takeFocus();
private:
    SETTING_TYPE GetSettingType(const QVariant& variant);
    void themeChanged();
    void setupLayout();

    QToolBar* toolbar = 0;
    QAction* submit_action = 0;
    DataEditWidget* edit_widget = 0;
};

#endif //POPUPDATAEDITOR_H
