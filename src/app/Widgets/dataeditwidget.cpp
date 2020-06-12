#include "dataeditwidget.h"
#include "../theme.h"

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

DataEditWidget::DataEditWidget(const QString& label, SETTING_TYPE type, const QVariant& data, QWidget *parent)
	: QWidget(parent)
{
    name = label;
    currentData = data;
    newData = data;
    this->type = type;

    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &DataEditWidget::themeChanged);
    themeChanged();
}

void DataEditWidget::updateIcon()
{
    Theme* theme = Theme::theme();
    if (icon_path_.first != "" && icon_path_.second != "") {
        if (icon_action) {
            icon_action->setIcon(theme->getIcon(icon_path_));
        }
        setIconVisible(true);
    }
}

void DataEditWidget::setIcon(const QString& icon_path, const QString& icon_name)
{
	icon_path_.first = icon_path;
	icon_path_.second = icon_name;
    updateIcon();
}

void DataEditWidget::setIconVisible(bool visible)
{
    if (icon_action && icon_action != button_action) {
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
    if (label) {
        return label->fontMetrics().horizontalAdvance(label->text());
    }
    return -1;
}

void DataEditWidget::setLabelWidth(int width)
{
    if (label) {
        label->setFixedWidth(width);
    }
}

void DataEditWidget::setValue(const QVariant& data)
{
	// This sets newData to data
    dataChanged(data);
	
	// INSPECT: Nothing happens in the cases where the casted edit widget is null or if the newData is invalid
	//  Is that intentional???

    switch(type){
    case SETTING_TYPE::BOOL:{
        auto check_box = qobject_cast<QCheckBox*>(editWidget_);
        if(check_box){
            check_box->setChecked(newData.toBool());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        auto spin_box = qobject_cast<QSpinBox*>(editWidget_);
        if(spin_box){
            spin_box->setValue(newData.toInt());
        }
        break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = qobject_cast<QSlider*>(editWidget_);
        if(slider){
            slider->setValue(newData.toInt());
        }
        break;
    }
    case SETTING_TYPE::COLOR:{
        auto lineEdit = qobject_cast<QLineEdit*>(editWidget_);
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
        auto line_edit = qobject_cast<QLineEdit*>(editWidget_);
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
    if (isHighlighted) {
        label_ss = "color: " + theme->getHighlightColorHex() + "; text-decoration:underline;";
    }

    // set the children QLabel's stylesheets - simply setting it for this widget doesn't work
    for (auto l : findChildren<QLabel*>()) {
        l->setStyleSheet(label_ss);
    }

	if (editWidget_) {
		auto line_edit = qobject_cast<QLineEdit*>(editWidget_);
		auto spin_box = qobject_cast<QSpinBox*>(editWidget_);
		auto slider = qobject_cast<QSlider*>(editWidget_);
		if (line_edit) {
			editWidget_->setStyleSheet(theme->getLineEditStyleSheet());
		} else if (spin_box) {
			editWidget_->setStyleSheet(theme->getLineEditStyleSheet("QSpinBox"));
		} else if (slider) {
			editWidget_->setStyleSheet(theme->getSliderStyleSheet());
		} else {
			auto style = label_ss;
			if (type != SETTING_TYPE::BOOL) {
				style += "background:" + Theme::theme()->getAltBackgroundColorHex() + "; border: 1px solid " +
						 theme->getAltBackgroundColorHex() + ";";
			}
			editWidget_->setStyleSheet(style);
		}
	}
	
    switch(type){
        case SETTING_TYPE::FILE:
            button_action->setIcon(theme->getIcon("Icons", "file"));
            break;
        case SETTING_TYPE::PATH:
            button_action->setIcon(theme->getIcon("Icons", "folder"));
            break;
        case SETTING_TYPE::COLOR: {
			auto line_edit = qobject_cast<QLineEdit*>(editWidget_);
			if (line_edit) {
				QColor color(line_edit->text());
				button_action->setIcon(theme->getImage("Icons", "drop", QSize(), color));
			}
			break;
		}
        default:
            break;
    }
    
    updateIcon();
}

void DataEditWidget::dataChanged(const QVariant& value)
{
    newData = value;
}

void DataEditWidget::editFinished()
{
	// This is very much like the setValue function
	// TODO: Investigate and refactor this and setValue
	// INSPECT: There are cases where calling dataChanged or setting the displayed value of the widget is unnecessary
	//  Those cases are handled using connections made for specific widgets in the setupLayout

    switch(type){
    case SETTING_TYPE::BOOL:{
        auto checkBox = qobject_cast<QCheckBox*>(editWidget_);
        if (checkBox) {
            //Call dataChanged first
            dataChanged(checkBox->isChecked());
        }
        break;
    }
    case SETTING_TYPE::INT:{
        auto spinBox = qobject_cast<QSpinBox*>(editWidget_);
        if (spinBox) {
            //Call data Changed first
            dataChanged(spinBox->value());
        }
        break;
    }
    case SETTING_TYPE::COLOR:{
        auto lineEdit = qobject_cast<QLineEdit*>(editWidget_);
        //Validate color;
		if (lineEdit) {
			QColor color(newData.toString());
			if (color.isValid()) {
				lineEdit->setText(newData.toString());
			}
			themeChanged();
		}
        break;
    }
    case SETTING_TYPE::STRINGLIST:{
        auto combo_box = qobject_cast<QComboBox*>(editWidget_);
        if (combo_box) {
            //Call data Changed first
            dataChanged(combo_box->currentText());
        }
        break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = qobject_cast<QSlider*>(editWidget_);
        if (slider) {
            //Call data Changed first
            dataChanged(slider->value());
        }
        break;
    }
    case SETTING_TYPE::STRING:
    case SETTING_TYPE::FILE:
    case SETTING_TYPE::PATH:{
        //Do the same things.
        auto line_edit = qobject_cast<QLineEdit*>(editWidget_);
        if (line_edit && line_edit->text() != newData.toString()) {
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

QAction* DataEditWidget::getButtonAction()
{
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
    toolbar->addWidget(label);

    bool needs_label = true;
    bool uses_button = false;

    switch(type){
        case SETTING_TYPE::STRING:{
            auto line_edit = new QLineEdit(this);
            line_edit->setText(currentData.toString());
            connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
            connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);
			editWidget_ = line_edit;
            toolbar->addWidget(line_edit);
        break;
    }
    case SETTING_TYPE::STRINGLIST:{
        auto combo_box = new QComboBox(this);
        combo_box->addItems(currentData.toStringList());
        connect(combo_box, &QComboBox::currentTextChanged, this, &DataEditWidget::editFinished);
        toolbar->addWidget(combo_box);
		editWidget_ = combo_box;
		break;
    }
    case SETTING_TYPE::PERCENTAGE:{
        auto slider = new QSlider(Qt::Horizontal, this);
        slider->setMinimum(0);
        slider->setMaximum(100);
        slider->setValue(currentData.toInt());
		toolbar->addWidget(slider);
		
		auto value_label = new QLabel(this);
        connect(slider, &QSlider::valueChanged, [=](int value){
            value_label->setText(QStringLiteral("%1").arg(value).leftJustified(3, ' ') + QString("%") );
        });
		emit slider->valueChanged(slider->value());
		toolbar->addWidget(value_label);
	
		connect(slider, &QSlider::valueChanged, this, &DataEditWidget::editFinished);
		editWidget_ = slider;
        break;
    }
    case SETTING_TYPE::BOOL:{
        needs_label = false;
        auto check_box = new QCheckBox(name, this);
        check_box->setChecked(currentData.toBool());
        connect(check_box, &QCheckBox::clicked, this, &DataEditWidget::editFinished);
        toolbar->addWidget(check_box);
		editWidget_ = check_box;
		break;
    }
    case SETTING_TYPE::INT:{
        auto spin_box = new QSpinBox(this);
        spin_box->setMaximum(10000);
        spin_box->setMinimum(-10000);
        spin_box->setValue(currentData.toInt());
		connect(spin_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DataEditWidget::editFinished);
		spin_box->setButtonSymbols(QAbstractSpinBox::NoButtons);
		toolbar->addWidget(spin_box);
		editWidget_ = spin_box;
		break;
    }
    case SETTING_TYPE::PATH:
    case SETTING_TYPE::FILE:{
        auto line_edit = new QLineEdit(this);
        line_edit->setText(currentData.toString());
		connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
		connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);
		toolbar->addWidget(line_edit);
		button_action = toolbar->addAction("Select File/Path");
		connect(button_action, &QAction::triggered, this, &DataEditWidget::pickPath);
		editWidget_ = line_edit;
		break;
    }
    case SETTING_TYPE::COLOR:{
        auto line_edit = new QLineEdit(this);
        line_edit->setText(currentData.toString());
		connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
		connect(line_edit, &QLineEdit::textChanged, this, &DataEditWidget::editFinished);
		toolbar->addWidget(line_edit);
		button_action = toolbar->addAction("Select Color");
		connect(button_action, &QAction::triggered, this, &DataEditWidget::pickColor);
		editWidget_ = line_edit;
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

    if (uses_button) {
        delete icon_action;
        icon_action = button_action;
        auto tool_button = qobject_cast<QToolButton*>(toolbar->widgetForAction(button_action));
        tool_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        auto tool_button = qobject_cast<QToolButton*>(toolbar->widgetForAction(icon_action));
        if (tool_button) {
            tool_button->setAutoRaise(false);
            tool_button->setStyleSheet("QToolButton{background:none;border:none;}");
        }
    }
    if (!needs_label) {
        delete label;
        label = nullptr;
    }

    if (editWidget_) {
        editWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        editWidget_->setAttribute(Qt::WA_MacShowFocusRect, false);
        if (label) {
            label->setFocusPolicy(Qt::ClickFocus);
            label->setFocusProxy(editWidget_);
        }
    }
}