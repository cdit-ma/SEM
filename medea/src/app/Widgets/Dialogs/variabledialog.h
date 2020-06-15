#ifndef VARIABLE_DIALOG_H
#define VARIABLE_DIALOG_H

#include "popupwidget.h"
#include "../dataeditwidget.h"
#include "../customgroupbox.h"

#include <QToolBar>

class VariableDialog : public PopupWidget
{
    Q_OBJECT

public:
    explicit VariableDialog(QString label);

    void addOption(const QString& option, SETTING_TYPE type, const QVariant& default_value = QVariant());
    void setOptionIcon(const QString& option, const QString& icon_path, const QString& icon_alias);
    void setOptionEnabled(const QString& option, bool enabled);

    QVariant getOptionValue(const QString& option);
    QHash<QString, QVariant> getOptions();

private:
    void setupLayout();
    void themeChanged();

    QAction* cancel_action = nullptr;
    QAction* okay_action = nullptr;
    QToolBar* toolbar = nullptr;
    CustomGroupBox* box = nullptr;

    QString label;
    QHash<QString, DataEditWidget*> fields;
};

#endif // VARIABLE_DIALOG_H