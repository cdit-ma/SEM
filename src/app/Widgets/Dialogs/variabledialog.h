#ifndef VARIABLE_DIALOG_H
#define VARIABLE_DIALOG_H

#include "popupwidget.h"
#include "../dataeditwidget.h"
#include "../customgroupbox.h"
#include <QToolBar>

class VariableDialog:  public PopupWidget
{
    Q_OBJECT
public:
    VariableDialog(QString label);
    void addOption(QString option, SETTING_TYPE type, QVariant default_value = QVariant());
    void setOptionIcon(QString option, QString icon_path, QString icon_alias);
    void setOptionEnabled(QString option, bool enabled);
    QVariant getOptionValue(QString option);

    QHash<QString, QVariant> getOptions();
private:
    void setupLayout();
    void themeChanged();

    QHash<QString, DataEditWidget*> fields;
    QAction* cancel_action = 0;
    QAction* okay_action = 0;
    QToolBar* toolbar = 0;

    CustomGroupBox* box = 0;
    QString label;
};

#endif //VARIABLE_DIALOG_H
