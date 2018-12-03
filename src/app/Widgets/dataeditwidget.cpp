#include "dataeditwidget.h"
#include <QVBoxLayout>


#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QSlider>
#include "../theme.h"

DataEditWidget::DataEditWidget(QString label, SETTING_TYPE type, QVariant data, QWidget *parent) : QWidget(parent)
{

    this->name = label;
    this->currentData = data;
    this->newData = data;
    this->type = type;

    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataEditWidget::themeChanged);
    themeChanged();
}
void DataEditWidget::updateIcon(){
    Theme* theme = Theme::theme();
    if(icon_path.first != "" && icon_path.second != ""){
        if(icon_action){
            icon_action->setIcon(theme->getIcon(icon_path));
        }
        setIconVisible(true);
    }
}

void DataEditWidget::setIcon(QString path, QString name){
    icon_path.first = path;
    icon_path.second = name;
    updateIcon();
    
}
void DataEditWidget::setIconVisible(bool visible){
    if(icon_action && icon_action != button_action){
        icon_action->setVisible(visible);
    }
}



SETTING_TYPE DataEditWidget::getType()
{
    return type;
}

void DataEditWidget::setHighlighted(bool highlighted)
{
    isHighlighted = highlighted;
    themeChanged();
}

int DataEditWidget::getMinimumLabelWidth()
{
    if(label){
        return label->fontMetrics().width(label->text());
    }
    return -1;
}

void DataEditWidget::setLabelWidth(int width)
{
    if(label){
        label->setFixedWidth(width);
    }
}

void DataEditWidget::setValue(QVariant data)
{
    dataChanged(data);

    switch(type){
    case SETTING_TYPE::BOOL:{
        auto check_box = qobject_cast<QCheckBox*>(editWidget_1);
        if(check_box){
            check_box->setChecked(newData.toBool());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        auto spin_box = qobject_cast<QSpinBox*>(editWidget_1);
        if(spin_box){
            spin_box->setValue(newData.toInt());
        }
        break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = qobject_cast<QSlider*>(editWidget_1);
        if(slider){
            slider->setValue(newData.toInt());
        }
        break;
    }
    case SETTING_TYPE::COLOR:{
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);

        //Validate color;
        QColor color(newData.toString());
        if(color.isValid()){
            if(lineEdit){
                lineEdit->setText(newData.toString());
            }
            themeChanged();
        }
        break;
    }
    case SETTING_TYPE::FILE:
    case SETTING_TYPE::PATH:
    case SETTING_TYPE::STRING:{
        auto line_edit = qobject_cast<QLineEdit*>(editWidget_1);
        if(line_edit){
            line_edit->setText(newData.toString());
        }
        break;
    }
    default:
        break;

    }

    currentData = newData;
    emit valueChanged(newData);
}

QVariant DataEditWidget::getValue()
{
    return currentData;
}

void DataEditWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    
    if(toolbar){
        auto icon_size = theme->getIconSize();
        toolbar->setMinimumHeight(icon_size.height());
        toolbar->setStyleSheet(theme->getToolBarStyleSheet());
        toolbar->setIconSize(icon_size);
    }
    auto label_ss = "color: " + theme->getTextColorHex() + ";";
    
    
    if(isHighlighted){
        label_ss = "color: " + theme->getHighlightColorHex() + "; text-decoration:underline;";
    }
    setStyleSheet("QLabel{" + label_ss + "}");

    auto line_edit = qobject_cast<QLineEdit*>(editWidget_1);
    auto spin_box = qobject_cast<QSpinBox*>(editWidget_1);
    auto slider = qobject_cast<QSlider*>(editWidget_1);

    if(line_edit){
        line_edit->setStyleSheet(theme->getLineEditStyleSheet());
    }else if(spin_box){
        editWidget_1->setStyleSheet(theme->getLineEditStyleSheet("QSpinBox"));
    }else if(slider){
        editWidget_1->setStyleSheet(theme->getSliderStyleSheet());
    }else if(editWidget_1){
        auto style = label_ss;
        if(type != SETTING_TYPE::BOOL){
            style += "background:" + Theme::theme()->getAltBackgroundColorHex() + "; border: 1px solid " + theme->getAltBackgroundColorHex() + ";";
        }
        editWidget_1->setStyleSheet(style);
    }

    switch(type){
        case SETTING_TYPE::FILE:{
            button_action->setIcon(theme->getIcon("Icons", "file"));
            break;
        }
        case SETTING_TYPE::PATH:{
            button_action->setIcon(theme->getIcon("Icons", "folder"));
            break;
        }
        case SETTING_TYPE::COLOR:{
            auto line_edit = qobject_cast<QLineEdit*>(editWidget_1);
            if(line_edit){
                QColor color(line_edit->text());
                button_action->setIcon(theme->getImage("Icons", "drop", QSize(), color));
            }
            break;
        }
        default:{
            break;
        }
    }
    updateIcon();
}

void DataEditWidget::dataChanged(QVariant value)
{
    newData = value;
}

void DataEditWidget::editFinished()
{
    switch(type){
    case SETTING_TYPE::BOOL:{
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(editWidget_1);
        if(checkBox){

            //Call dataChanged first
            dataChanged(checkBox->isChecked());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editWidget_1);
        if(spinBox){
            //Call data Changed first
            dataChanged(spinBox->value());
        }
        break;
    }
    case SETTING_TYPE::COLOR:{

        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);

        //Validate color;
        QColor color(newData.toString());
        if(lineEdit){
            lineEdit->setText(newData.toString());
        }
        themeChanged();
        break;
    }
    case SETTING_TYPE::STRINGLIST:{
        auto combo_box = qobject_cast<QComboBox*>(editWidget_1);
        if(combo_box){
            //Call data Changed first
            dataChanged(combo_box->currentText());
        }
        break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = qobject_cast<QSlider*>(editWidget_1);
        if(slider){
            //Call data Changed first
            dataChanged(slider->value());
        }
        break;
    }
    case SETTING_TYPE::STRING:
    case SETTING_TYPE::FILE:
    case SETTING_TYPE::PATH:{
        //Do the same things.
        auto line_edit = qobject_cast<QLineEdit*>(editWidget_1);
        if(line_edit && line_edit->text() != newData.toString()){
            line_edit->setText(newData.toString());
        }
        break;
    }
    default:
        break;

    }

    currentData = newData;
    emit valueChanged(newData);
}

void DataEditWidget::pickColor()
{
    //Get the current Color from the string.
    QColor currentColor(currentData.toString());

    //Select the new Color.
    QColor newColor  = QColorDialog::getColor(currentColor);
    if(newColor.isValid()){
        dataChanged(newColor.name());
        editFinished();
    }
}

void DataEditWidget::pickFont()
{
    //Get the current Color from the string.
    QFont current_font(currentData.value<QFont>());

    bool okay = false;
    //Select the new Color.
    auto new_font  = QFontDialog::getFont(&okay, current_font);
    if(okay){
        dataChanged(new_font);
        editFinished();
    }
}

QAction* DataEditWidget::getButtonAction(){
    auto button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    auto action = toolbar->addWidget(button);
    //Connect the button to it's action so we don't need to worry about QToolButton stuff
    button->setDefaultAction(action);
    return action;
}

void DataEditWidget::pickPath()
{
    QString path = "";
    switch(type){
        case SETTING_TYPE::PATH:
        path = QFileDialog::getExistingDirectory(this, "Select Path" , currentData.toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        break;
    case SETTING_TYPE::FILE:
        path = QFileDialog::getOpenFileName(this, "Select File" , currentData.toString());
        break;
    default:
        return;
    }
    if(path != ""){
        dataChanged(path);
        editFinished();
    }
}

void DataEditWidget::setupLayout()
{
    toolbar = new QToolBar(this);
    
    
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto hbox_layout = new QHBoxLayout(this);
    hbox_layout->setMargin(2);
    hbox_layout->addWidget(toolbar);

    icon_action = toolbar->addAction("");
    icon_action->setVisible(false);

    label = new QLabel(name, this);
    auto label_action = toolbar->addWidget(label);

    bool needs_label = true;
    bool uses_button = false;

    switch(type){
        case SETTING_TYPE::STRING:{
            auto line_edit = new QLineEdit(this);
            line_edit->setText(currentData.toString());
            connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
            connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);
            editWidget_1 = line_edit;
            toolbar->addWidget(line_edit);
        break;
    }
    case SETTING_TYPE::STRINGLIST:{
        auto combo_box = new QComboBox(this);
        combo_box->addItems(currentData.toStringList());
        connect(combo_box, &QComboBox::currentTextChanged, this, &DataEditWidget::editFinished);
        
        editWidget_1 = combo_box;
        toolbar->addWidget(combo_box);
        break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = new QSlider(Qt::Horizontal, this);
        slider->setMinimum(0);
        slider->setMaximum(100);
        slider->setValue(currentData.toInt());
        auto value_label = new QLabel(this);

        toolbar->addWidget(slider);
        toolbar->addWidget(value_label);
        connect(slider, &QSlider::valueChanged, [=](int value){
            value_label->setText(QStringLiteral("%1").arg(value).leftJustified(3, ' ') + QString("%") );
        });
        emit slider->valueChanged(slider->value());
        
        connect(slider, &QSlider::valueChanged, this, &DataEditWidget::editFinished);
        
        editWidget_1 = slider;
        break;
    }
    case SETTING_TYPE::BOOL:{
        needs_label = false;
        auto check_box = new QCheckBox(name, this);
        check_box->setChecked(currentData.toBool());

        connect(check_box, &QCheckBox::clicked, this, &DataEditWidget::editFinished);

        editWidget_1 = check_box;
        toolbar->addWidget(check_box);
        break;
    }
    case SETTING_TYPE::INT:{
        auto spin_box = new QSpinBox(this);
        spin_box->setMaximum(10000);
        spin_box->setMinimum(-10000);
        spin_box->setValue(currentData.toInt());

        editWidget_1 = spin_box;

        connect(spin_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DataEditWidget::editFinished);
        spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        toolbar->addWidget(spin_box);
        break;
    }
    case SETTING_TYPE::PATH:
    case SETTING_TYPE::FILE:{
        auto line_edit = new QLineEdit(this);
        line_edit->setText(currentData.toString());

        toolbar->addWidget(line_edit);
        button_action = toolbar->addAction("Select File/Path");


        editWidget_1 = line_edit;

        connect(button_action, &QAction::triggered, this, &DataEditWidget::pickPath);
        connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
        connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);
        //connect(line_edit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);
        break;
    }
    case SETTING_TYPE::COLOR:{
        auto line_edit = new QLineEdit(this);
        line_edit->setText(currentData.toString());
        toolbar->addWidget(line_edit);
        button_action = toolbar->addAction("Select Color");

        editWidget_1 = line_edit;

        connect(button_action, &QAction::triggered, this, &DataEditWidget::pickColor);
        connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
        connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);

        break;
    }
    case SETTING_TYPE::FONT:{
        uses_button = true;
        needs_label = false;
        button_action = getButtonAction();
        button_action->setText(name);

        connect(button_action, &QAction::triggered, this, &DataEditWidget::pickFont);
        break;
    }
    case SETTING_TYPE::BUTTON:{
        uses_button = true;
        needs_label = false;
        button_action = getButtonAction();
        button_action->setText(name);

        connect(button_action, &QAction::triggered, this, &DataEditWidget::editFinished);
        break;
    }
    default:
        break;
    }

    

    if(uses_button){
        delete icon_action;
        icon_action = button_action;
        auto tool_button = qobject_cast<QToolButton*>(toolbar->widgetForAction(button_action));
        tool_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }else{
        auto tool_button = qobject_cast<QToolButton*>(toolbar->widgetForAction(icon_action));
        if(tool_button){
            tool_button->setAutoRaise(false);
            tool_button->setStyleSheet("QToolButton{background:none;border:none;}");
        }
    }
    if(!needs_label){
        delete label;
        label = 0;
    }

    if(editWidget_1){
        editWidget_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        editWidget_1->setAttribute(Qt::WA_MacShowFocusRect, false);
        if(label){
            label->setFocusPolicy(Qt::ClickFocus);
            label->setFocusProxy(editWidget_1);
        }
    }
}
