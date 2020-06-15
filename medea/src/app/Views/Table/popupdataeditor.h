#ifndef POPUPDATAEDITOR_H
#define POPUPDATAEDITOR_H

#include "../../Widgets/Dialogs/popupwidget.h"
#include "../../Widgets/dataeditwidget.h"
#include "../../Controllers/ViewController/viewitem.h"

#include <QToolBar>

class PopupDataEditor : public PopupWidget
{
    Q_OBJECT
    
public:
    explicit PopupDataEditor();
    
    void edit(ViewItem* view_item, const QString& key_name);
    
private:
    SETTING_TYPE GetSettingType(const QVariant& variant);
    
    void themeChanged();
    void setupLayout();

    QToolBar* toolbar = nullptr;
    QAction* submit_action = nullptr;
    DataEditWidget* edit_widget = nullptr;
};

#endif //POPUPDATAEDITOR_H
