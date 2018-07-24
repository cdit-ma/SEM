
#include "variabledialog.h"
#include "../../theme.h"
#include "../../Controllers/WindowManager/windowmanager.h"

VariableDialog::VariableDialog(QString label):PopupWidget(PopupWidget::TYPE::DIALOG, 0){
    this->label = label;
    setupLayout();
    connect(Theme::theme(), &Theme::theme_Changed, this, &VariableDialog::themeChanged);
    themeChanged();
    setMouseTracking(true);
}

void VariableDialog::addOption(QString option, SETTING_TYPE type, QVariant default_value){
    if(!fields.contains(option)){
        auto edit_widget = new DataEditWidget(option, type, default_value, this);
        box->addWidget(edit_widget);
        fields.insert(option, edit_widget);
    }
}

void VariableDialog::setOptionIcon(QString option, QString icon_path, QString icon_alias){
    auto edit_widget = fields.value(option, 0);
    if(edit_widget){
        edit_widget->setIcon(icon_path, icon_alias);
    }
}

QVariant VariableDialog::getOptionValue(QString option){
    auto edit_widget = fields.value(option, 0);
    if(edit_widget){
        return edit_widget->getValue();
        
    }
    return QVariant();
}

void VariableDialog::setOptionEnabled(QString option, bool enabled){
    auto edit_widget = fields.value(option, 0);
    if(edit_widget){
        edit_widget->setEnabled(enabled);
    }
}

QHash<QString, QVariant> VariableDialog::getOptions(){
    QHash<QString, QVariant> values;
    

    WindowManager::MoveWidget(this);
    if(exec()){
        for(auto key : fields.keys()){
            auto value = fields[key]->getValue();
            values.insert(key, value);
        }
    }
    return values;
}

void VariableDialog::setupLayout(){
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
  

    box = new CustomGroupBox(label);
    box->setCheckable(false);
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    cancel_action = toolbar->addAction("Cancel");
    okay_action = toolbar->addAction("Launch");

    layout->addWidget(box, 1);
    layout->addWidget(toolbar, 0, Qt::AlignRight);
    setWidget(widget);

    connect(okay_action, &QAction::triggered, this, &PopupWidget::accept);
    connect(cancel_action, &QAction::triggered, this, &PopupWidget::reject);
}

void VariableDialog::themeChanged(){
    Theme* theme = Theme::theme();
    cancel_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    okay_action->setIcon(theme->getIcon("Icons", "circleTickDark"));
    toolbar->setIconSize(theme->getIconSize());
    toolbar->setStyleSheet(theme->getToolBarStyleSheet());
}