#include "iconpicker.h"
#include "../theme.h"
#include <QDebug>
#include <QGridLayout>
#include <QToolBar>
#include <QScrollArea>




IconPicker::IconPicker(QWidget *parent) : QFrame(parent)
{
    setMinimumSize(520,700);
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &IconPicker::themeChanged);
    themeChanged();

    //setupLayout();
    
}


void IconPicker::themeChanged()
{
    if(!setup_layout){
        return;
    }
    auto theme = Theme::theme();

    setStyleSheet(
                    "IconPicker {background-color: " % theme->getBackgroundColorHex() + ";border:1px solid " % theme->getDisabledBackgroundColorHex() % ";}" +
                    "QScrollArea {border: 1px solid " % theme->getAltBackgroundColorHex() % "; background: rgba(0,0,0,0); } " +
                    theme->getToolBarStyleSheet() + 
                    theme->getScrollBarStyleSheet() +
                    theme->getLineEditStyleSheet() + 
                    theme->getLabelStyleSheet() +
                    "QWidget#BLANK_WIDGET{background: rgba(0,0,0,0);}"
                    
                );

    label_selected_icon->setStyleSheet("border:1px solid " % theme->getAltBackgroundColorHex() % ";");
    apply_action->setIcon(theme->getIcon("Icons", "tick"));
    
    for(auto action : all_icons){
        Theme::UpdateActionIcon(action, theme);
    }

    updateIcon();
}

void IconPicker::clear(){
    if(setup_layout){
        edit_icon->clear();
        edit_icon_prefix->clear();
    }
}

void IconPicker::setCurrentIcon(QString icon_prefix, QString icon_name){
    if(setup_layout){
        edit_icon_prefix->setText(icon_prefix);
        edit_icon->setText(icon_name);
    }
}

void IconPicker::setupLayout()
{
    auto theme = Theme::theme();

    auto icon_size = theme->getLargeIconSize();
    auto column_count = 8;

    int row = 0;
    int column = column_count;

    QToolBar* toolbar = 0;
    auto layout = new QVBoxLayout(this);
    

    auto scroll_widget = new QWidget(this);
    scroll_widget->setObjectName("BLANK_WIDGET");
    scroll_widget->setContentsMargins(5,0,5,5);
    auto v_layout = new QVBoxLayout(scroll_widget);
    v_layout->setMargin(0);
    v_layout->setAlignment(Qt::AlignTop);
    v_layout->setMargin(0);
    v_layout->setSpacing(0);

    auto scroll_area = new QScrollArea(this);
    scroll_area->setWidget(scroll_widget);
    scroll_area->setWidgetResizable(true);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    
    label_selected_icon = new QLabel(this);
    label_selected_icon->setScaledContents(false);
    label_selected_icon->setAlignment(Qt::AlignCenter);
    label_selected_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    label_icon_prefix = new QLabel("Icon Prefix:", this);
    label_icon = new QLabel("Icon:", this);

    edit_icon_prefix = new QLineEdit(this);
    edit_icon = new QLineEdit(this);
    QToolBar* apply_toolbar = new QToolBar(this);
    apply_toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    apply_toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    apply_toolbar->setIconSize(preview_icon_size);

    apply_action = apply_toolbar->addAction("Apply");


    connect(edit_icon_prefix, &QLineEdit::textChanged, this, &IconPicker::updateIcon);
    connect(edit_icon, &QLineEdit::textChanged, this, &IconPicker::updateIcon);

    edit_icon_prefix->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    {
        auto h_layout = new QGridLayout();
        h_layout->setMargin(0);
        h_layout->setSpacing(5);

        
        h_layout->addWidget(label_selected_icon,    0, 0, 2, 2);
        h_layout->addWidget(label_icon_prefix,      0, 2, 1, 1, Qt::AlignRight);
        h_layout->addWidget(edit_icon_prefix,       0, 3, 1, 1);

        h_layout->addWidget(label_icon,             1, 2, 1, 1, Qt::AlignRight);
        h_layout->addWidget(edit_icon,              1, 3, 1, 1);

        

        h_layout->addWidget(apply_toolbar,           0, 4, 2, 2);

        layout->addLayout(h_layout);

        h_layout->setColumnStretch(3, 1);
    }
   

    layout->addWidget(scroll_area, 1);





    auto prefix_list = theme->getIconPrefixs();
    std::sort(prefix_list.rbegin(), prefix_list.rend());

    for(auto icon_prefix : prefix_list){
        auto icon_list = theme->getIcons(icon_prefix);
        std::sort(icon_list.begin(), icon_list.end());
        for(auto icon : icon_list){
            if(column >= column_count){
                toolbar = new QToolBar(this);
                toolbar->setOrientation(Qt::Horizontal);
                toolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                toolbar->setIconSize(icon_size);
                v_layout->addWidget(toolbar, 0, Qt::AlignHCenter);
                column = 0;
            }

            if(theme->gotImage(icon_prefix, icon)){
                auto action = toolbar->addAction(icon);
                all_icons.insert(action);

                

                action->setProperty("icon_name", icon);
                action->setProperty("icon_prefix", icon_prefix);
                Theme::StoreActionIcon(action, icon_prefix, icon);
                connect(action, &QAction::triggered, this, [=](){iconPressed(action);});
                action->setCheckable(true);
                column ++;
            }
        }
    }

    layout->addStretch();

    connect(apply_action, &QAction::triggered, this, &IconPicker::ApplyIcon);

    setup_layout = true;
    themeChanged();
}

void IconPicker::clearSelection(){
    for(auto icon : selected_icons){
        icon->setChecked(false);
    }
    selected_icons.clear();
}

void IconPicker::updateIcon(){
    auto icon_prefix = edit_icon_prefix->text();
    auto icon_name = edit_icon->text();

    auto theme = Theme::theme();

    bool got_valid = true;

    if(!theme->gotImage(icon_prefix, icon_name)){
        icon_prefix = "Icons";
        icon_name = "circleDark";
        got_valid = false;
    }
    
    auto pixmap = theme->getImage(icon_prefix, icon_name, preview_icon_size, theme->getMenuIconColor(ColorRole::NORMAL));
    label_selected_icon->setPixmap(pixmap);
    clearSelection();
    
    if(got_valid){
        //Try and check to see
        for(auto action : all_icons){
            auto action_icon_prefix = action->property("icon_prefix").toString();
            auto action_icon_name = action->property("icon_name").toString();
            if(icon_prefix == action_icon_prefix && icon_name == action_icon_name){
                iconPressed(action);
            }
        }
    }
}

void IconPicker::iconPressed(QAction* action){
    clearSelection();
    selected_icons.insert(action);
    action->setChecked(true);

    edit_icon->setText(action->property("icon_name").toString());
    edit_icon_prefix->setText(action->property("icon_prefix").toString());
}


QPair<QString, QString> IconPicker::getCurrentIcon(){
    QPair<QString, QString> selected_icon;

    if(setup_layout){
        if(selected_icons.size()){
            for(auto icon : selected_icons){
                selected_icon.first = icon->property("icon_prefix").toString();
                selected_icon.second = icon->property("icon_name").toString();
            }
        }else{
            selected_icon.first = edit_icon->text();
            selected_icon.second = edit_icon_prefix->text();
        }
    }

    return selected_icon;
}

void IconPicker::showEvent(QShowEvent *event){
    if(!setup_layout){
        setupLayout();
    }
}