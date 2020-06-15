#include "popupdataeditor.h"
#include "../../theme.h"

#include <QAbstractItemView>
#include <QStyledItemDelegate>

PopupDataEditor::PopupDataEditor()
	: PopupWidget(PopupWidget::TYPE::POPUP, nullptr)
{
    setupLayout();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &PopupDataEditor::themeChanged);
    themeChanged();

    hide();
}

void PopupDataEditor::edit(ViewItem* view_item, const QString& key_name)
{
    if(edit_widget){
        delete edit_widget;
        edit_widget = nullptr;
    }

    auto value = view_item->getData(key_name);
    auto type = GetSettingType(value);

    edit_widget = new DataEditWidget(key_name, type, value, this);
    toolbar->insertWidget(submit_action, edit_widget);
    
    show();
}

SETTING_TYPE PopupDataEditor::GetSettingType(const QVariant& variant)
{
    SETTING_TYPE return_type = SETTING_TYPE::STRING;
    auto type = variant.type();
    
    if(type == QVariant::Bool){
        return_type = SETTING_TYPE::BOOL;
    }else if(type == QVariant::Int){
        return_type = SETTING_TYPE::INT;
    }

    return return_type;
}

void PopupDataEditor::themeChanged()
{
    auto theme = Theme::theme();

    toolbar->setStyleSheet(theme->getToolBarStyleSheet());
	toolbar->setIconSize(theme->getIconSize());
	
	submit_action->setIcon(theme->getIcon("Icons", "tickCircleDark"));
}

void PopupDataEditor::setupLayout()
{
    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedWidth(300);
    toolbar->setStyleSheet("QToolBar{padding:2px;}");

    submit_action = toolbar->addAction("Submit");
    setWidget(toolbar);
}