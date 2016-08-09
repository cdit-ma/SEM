#include "dataeditwidget.h"
#include <QVBoxLayout>


#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QColorDialog>
#include "../../View/theme.h"

#define SMALL_SQUARE 24
DataEditWidget::DataEditWidget(QString dataKey, QString label, SETTING_TYPE type, QVariant data, QWidget *parent) : QWidget(parent)
{
    isHighlighted = false;
    this->dataKey = dataKey;
    this->label = label;
    this->currentData = data;
    this->type = type;

    setContentsMargins(0,0,0,0);
    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

QString DataEditWidget::getKeyName()
{
    return dataKey;
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
    if(editLabel){
        return editLabel->fontMetrics().width(editLabel->text());
    }
    return -1;
}

void DataEditWidget::setLabelWidth(int width)
{
    if(editLabel){
        editLabel->setFixedWidth(width);
    }
}

void DataEditWidget::setValue(QVariant data)
{
    dataChanged(data);
    editFinished();
}

void DataEditWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    QPushButton* button = qobject_cast<QPushButton*>(editWidget_2);
    if(button){
        QString style = theme->getPushButtonStyleSheet();
        //QString style = "border:1px solid " + theme->getAltBackgroundColorHex() + ";";
        switch(type){
        case ST_FILE:{
            button->setIcon(Theme::theme()->getIcon("Actions", "New"));
            break;
        }
        case ST_PATH:{
            button->setIcon(Theme::theme()->getIcon("Actions", "Open"));
            break;
        }
        case ST_COLOR:{
            style += "QPushButton{background: " + currentData.toString() + ";}";
            break;
        }
        default:{
            break;
        }

        }
        button->setStyleSheet(style);
    }

    QString labelStyleSheet = "";

    if(isHighlighted){
        labelStyleSheet += "color: " + theme->getHighlightColorHex() + "; text-decoration:underline;";
    }else{
        labelStyleSheet += "color: " + theme->getTextColorHex() + "; text-decoration:normal;";
    }

    if(editLabel){
        editLabel->setStyleSheet(labelStyleSheet);
    }



    if(editWidget_1){
        QString style = "color:" + Theme::theme()->getTextColorHex() + ";";
        if(type == ST_BOOL){
            style = labelStyleSheet;
        }else{
            style += "background:" + Theme::theme()->getAltBackgroundColorHex() + "; border: 1px solid " + theme->getAltBackgroundColorHex() + ";";
        }

        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
        QPushButton* button = qobject_cast<QPushButton*>(editWidget_1);
        if(lineEdit){
            lineEdit->setStyleSheet(theme->getLineEditStyleSheet());
        }else if(button){
            button->setStyleSheet(theme->getPushButtonStyleSheet());
        }else{
            editWidget_1->setStyleSheet(style);
        }
    }
}

void DataEditWidget::dataChanged(QVariant value)
{
    newData = value;
}

void DataEditWidget::editFinished()
{
    switch(type){
    case ST_BOOL:{
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(editWidget_1);
        if(checkBox){
            //Call dataChanged first
            dataChanged(checkBox->isChecked());
        }
        break;
    }
    case ST_INT:{
        QSpinBox* spinBox = qobject_cast<QSpinBox*>(editWidget_1);
        if(spinBox){
            //Call data Changed first
            dataChanged(spinBox->value());
        }
        break;
    }
        case ST_COLOR:{

        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);

        //Validate color;
        QColor color(newData.toString());
        if(color.isValid()){
            if(lineEdit){
                lineEdit->setText(newData.toString());
            }
            themeChanged();
            break;
        }else{
            newData = currentData;
            lineEdit->setText(currentData.toString());
            return;
        }
        break;
    }
    case ST_FILE:
    case ST_PATH:{
        //Do the same things.
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editWidget_1);
        if(lineEdit){
            lineEdit->setText(newData.toString());
        }
        break;
    }
    default:
        break;

    }

    currentData = newData;
    emit valueChanged(dataKey, newData);
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

void DataEditWidget::pickPath()
{
    QString path = "";
    switch(type){
        case ST_PATH:
        path = QFileDialog::getExistingDirectory(this, "Select Path" , currentData.toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        break;
    case ST_FILE:
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
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    QHBoxLayout* editLayout = new QHBoxLayout();


    verticalLayout->setMargin(0);
    editLayout->setMargin(0);


    verticalLayout->addLayout(editLayout);

    editWidget_1 = 0;
    editWidget_2 = 0;
    editLabel = new QLabel(label, this);

    editLayout->addWidget(editLabel);

    switch(type){
        case ST_STRING:{
        QLineEdit* lineEdit = new QLineEdit(this);
        lineEdit->setText(currentData.toString());
        lineEdit->setFixedHeight(SMALL_SQUARE);


        connect(lineEdit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
        connect(lineEdit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);

        editWidget_1 = lineEdit;
        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case ST_BOOL:{
        QCheckBox* checkBox = new QCheckBox(label, this);

        checkBox->setChecked(currentData.toBool());


        connect(checkBox, &QCheckBox::clicked, this, &DataEditWidget::editFinished);

        editWidget_1 = checkBox;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(editWidget_1, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    case ST_INT:{
        QSpinBox* spinBox = new QSpinBox(this);
        spinBox->setMaximum(10000);
        spinBox->setValue(currentData.toInt());

        editWidget_1 = spinBox;

        connect(spinBox, &QSpinBox::editingFinished, this, &DataEditWidget::editFinished);

        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case ST_PATH:
    case ST_FILE:{
        QLineEdit* lineEdit = new QLineEdit(this);

        lineEdit->setText(currentData.toString());

        QPushButton* button = new QPushButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
        button->setFlat(true);

        editWidget_1 = lineEdit;
        editWidget_2 = button;
        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(editWidget_2);

        connect(button, &QPushButton::pressed, this, &DataEditWidget::pickPath);
        connect(lineEdit, &QLineEdit::textChanged, this, &DataEditWidget::dataChanged);
        connect(lineEdit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);
        break;
    }
    case ST_COLOR:{
        QLineEdit* lineEdit = new QLineEdit(this);
        lineEdit->setText(currentData.toString());



        QPushButton* button = new QPushButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
        button->setFlat(true);

        editWidget_1 = lineEdit;
        editWidget_2 = button;

        connect(button, &QPushButton::pressed, this, &DataEditWidget::pickColor);
        connect(lineEdit, &QLineEdit::textEdited, this, &DataEditWidget::dataChanged);
        connect(lineEdit, &QLineEdit::editingFinished, this, &DataEditWidget::editFinished);

        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(editWidget_2);
        break;
    }
    case ST_BUTTON:{
        QPushButton* button = new QPushButton(label, this);
        button->setFixedHeight(SMALL_SQUARE);
        connect(button, &QPushButton::clicked, this, &DataEditWidget::editFinished);

        editWidget_1 = button;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(editWidget_1, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    default:{



    }
    }

    if(editWidget_1){
        editWidget_1->setFixedHeight(SMALL_SQUARE);
    }
    if(editWidget_2){
        editWidget_2->setFixedHeight(SMALL_SQUARE);
    }



}
