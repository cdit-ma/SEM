#include "keyeditwidget.h"

#include <QHBoxLayout>
#include <QVariant>

#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>


#include <QLabel>
#include <QDebug>
#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>
#include <QEventLoop>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QGroupBox>
#include "../View/theme.h"

#define SMALL_SQUARE 25
KeyEditWidget::KeyEditWidget(QString g, QString k, QString keyNameHR, QVariant v, QString description, QString customType):QWidget(0)
{
    setObjectName("KeyEditWidget");
    groupName = g;
    keyName = k;
    hrKeyName = keyNameHR;
    descriptionBox = 0;
    difference = 0;
    value2Box = 0;

    newValue = QVariant();

    highlighted = false;

    bool isBool = false;
    bool isInt = false;
    bool isColor  = customType == "Color";
    isFilePath = customType == "File";
    isPath = customType == "Path";

    setStyleSheet("#HiddenGroup{border:0px;margin: 0px;padding: 0px;}");

    containerBox = new QGroupBox(this);
    containerBox->setObjectName("HiddenGroup");




    QString stringVal = v.toString();
    int intVal = -1;
    bool boolVal = false;
    QColor colorVal;

    if(!stringVal.isNull()){
        intVal = v.toInt(&isInt);
        if(stringVal == "true" || stringVal == "false"){
            isBool = true;
            boolVal = v.toBool();
        }
        if(isColor){
            //Convert to Color from Hex String.
            colorVal = QColor(stringVal);
        }
    }

    vLayout = new QVBoxLayout();

    vLayout->setSpacing(0);
    vLayout->setMargin(0);
    setLayout(vLayout);
    vLayout->addWidget(containerBox);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    containerBox->setLayout(hLayout);


    QString label = hrKeyName;
    if(!isBool){
        label += ":";
    }
    QPushButton* keyLabel = new QPushButton(label);
    keyLabel->setFlat(true);
    labelStyleSheet = "border:0px;text-align: left;";
    keyLabel->setStyleSheet(labelStyleSheet);

    if(description != ""){
        descriptionBox = new QTextBrowser() ;
        descriptionBox->setReadOnly(true);
        descriptionBox->setStyleSheet("QTextBrowser{font-size: 10px;color:#333;border:0px;}");
        descriptionBox->setHtml(description);
        descriptionBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        descriptionBox->setAttribute(Qt::WA_DontShowOnScreen);
        descriptionBox->show();

        int height = descriptionBox->document()->documentLayout()->documentSize().height();
        height +=  descriptionBox->document()->documentMargin() * 2;
        height +=  descriptionBox->contentsMargins().top() + descriptionBox->contentsMargins().bottom();
        descriptionBox->setFixedHeight(height);
        vLayout->addWidget(descriptionBox);
    }

    hLayout->addWidget(keyLabel);
    keyLabel->setFocusPolicy(Qt::NoFocus);
    hLayout->setSpacing(5);

    labelButton = keyLabel;

    colorBoxStyleSheet = "border:1px solid black;";
    if(isBool){
        //Setup Checkbox.
        QCheckBox* checkBox = new QCheckBox();
        checkBox->setChecked(boolVal);
        checkBox->setFixedHeight(SMALL_SQUARE);
        checkBox->setFixedWidth(SMALL_SQUARE);

        //Connect the key label to the checkbox.
        connect(keyLabel, SIGNAL(clicked()), checkBox, SLOT(click()));
        connect(checkBox, &QCheckBox::clicked, this, &KeyEditWidget::_valueChanged);
        connect(checkBox, SIGNAL(clicked()), this, SLOT(_editingFinished()));


        hLayout->insertWidget(0, checkBox);
        hLayout->setStretch(1, 1);

        keyLabel->setFocusPolicy(Qt::ClickFocus);
        valueBox = checkBox;
        value2Box = keyLabel;
        keyType = KEY_BOOL;
        oldValue = boolVal;
    }else if (isInt){
        QSpinBox* intEdit = new QSpinBox();
        intEdit->setRange(0,10000);
        intEdit->setValue(intVal);

        intEdit->setFixedHeight(SMALL_SQUARE);

        connect(intEdit, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KeyEditWidget::_valueChanged);
        connect(intEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));

        hLayout->addWidget(intEdit, 1);


        valueBox = intEdit;
        keyType = KEY_INT;
        oldValue = intVal;
    }else if(customType == "File"){
        QLineEdit* lineEdit = new QLineEdit(stringVal);
        lineEdit->setEnabled(false);
        hLayout->addWidget(lineEdit, 1);


        connect(lineEdit, &QLineEdit::textEdited, this, &KeyEditWidget::_valueChanged);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));

        valueBox = lineEdit;
        keyType = KEY_FILE;
        oldValue = stringVal;
    }else if(isColor){
        keyType = KEY_COLOR;

        QString hexName = colorVal.name();

        QLineEdit* hexEdit = new QLineEdit(hexName);
         hexEdit->setFixedHeight(SMALL_SQUARE);
        QPushButton* pickerButton = new QPushButton();

        pickerButton->setStyleSheet(colorBoxStyleSheet +"background:" + hexName +";");
        pickerButton->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
        pickerButton->setFlat(true);

        hLayout->addWidget(hexEdit, 1);
        hLayout->addWidget(pickerButton);

        valueBox = hexEdit;
        value2Box = pickerButton;

        connect(pickerButton, SIGNAL(pressed()), this, SLOT(pickColor()));
        connect(hexEdit, &QLineEdit::textEdited, this, &KeyEditWidget::_valueChanged);
        connect(hexEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));


        if(!colorVal.isValid()){
            //Set as null
            oldValue = QVariant();
        }else{
            oldValue = hexName;
        }
    }else{

        QLineEdit* lineEdit = new QLineEdit(stringVal);
        lineEdit->setFixedHeight(SMALL_SQUARE);
        hLayout->addWidget(lineEdit, 1);

        connect(lineEdit, &QLineEdit::textEdited, this, &KeyEditWidget::_valueChanged);

        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));

        if(isPath || isFilePath){
            QPushButton* pickerButton = new QPushButton();
            pickerButton->setStyleSheet(colorBoxStyleSheet +"background: #dddddd; ");
            pickerButton->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
            pickerButton->setFlat(true);

            if(isFilePath){
                pickerButton->setIcon(Theme::theme()->getImage("Actions", "New"));
            }else{
                pickerButton->setIcon(Theme::theme()->getImage("Actions", "Open"));
            }
            connect(pickerButton, SIGNAL(pressed()), this, SLOT(pickPath()));
            hLayout->addWidget(pickerButton);
        }
        oldValue = stringVal;
        valueBox = lineEdit;

        keyType = KEY_STRING;
    }

    setFixedHeight(vLayout->sizeHint().height());
}

QString KeyEditWidget::getKeyName()
{
    return keyName;
}

QString KeyEditWidget::getGroupName()
{
    return groupName;
}

QVariant KeyEditWidget::getValue()
{
    return oldValue;
}


int KeyEditWidget::getLabelWidth()
{
    if(labelButton && keyType != KEY_BOOL){
        QFont Font = labelButton->font();
        QFontMetrics fm(Font);
        return fm.width(hrKeyName);
    }
    return -1;
}

void KeyEditWidget::setLabelWidth(int width)
{
    if(labelButton && keyType != KEY_BOOL){
        labelButton->setFixedWidth(width + 2);
    }
}

void KeyEditWidget::setValue(QVariant value)
{
    if(keyType == KEY_INT){
        if(value.canConvert(QVariant::Int)){
            int intValue = value.toInt();

            QSpinBox* intEdit = dynamic_cast<QSpinBox*>(valueBox);
            if(intEdit){
                intEdit->setValue(intValue);
            }
        }
    }else if(keyType == KEY_BOOL){
        QString stringVal = value.toString();
        if(stringVal == "true" || stringVal == "false"){
            QCheckBox* boolEdit = dynamic_cast<QCheckBox*>(valueBox);
            if(boolEdit){
                boolEdit->setChecked(value.toBool());
            }
        }
    }else if(keyType == KEY_STRING || keyType == KEY_FILE){
        if(value.canConvert(QVariant::String)){
            QString valueStr = value.toString();
            QLineEdit* stringEdit = dynamic_cast<QLineEdit*>(valueBox);
            if(stringEdit){
                stringEdit->setText(valueStr);
            }
        }
    }else if(keyType == KEY_COLOR){
        QColor color = value.value<QColor>();
        updateColorWidget(color.name());
        value = color.name();
    }
    oldValue = value;
    //Update the value
}

void KeyEditWidget::setHighlighted(bool highlighted)
{
    if(highlighted){
        labelButton->setStyleSheet(labelStyleSheet + "text-decoration:underline;color:#ffa546;");
    }else{
        labelButton->setStyleSheet(labelStyleSheet + "text-decoration:normal;color:black;");
    }
}

void KeyEditWidget::updateColorWidget(QString color)
{
    if(keyType != KEY_COLOR){
        return;
    }
    QPushButton* button = dynamic_cast<QPushButton*>(value2Box);
    if(button){
        button->setStyleSheet(colorBoxStyleSheet +"background:" + color +";");
    }
    QLineEdit* stringEdit = dynamic_cast<QLineEdit*>(valueBox);
    if(stringEdit){
        stringEdit->setText(color);
    }
}

void KeyEditWidget::pickColor()
{
    //Get the current Color from the string.
    QColor currentColor(oldValue.toString());

    //Select the new Color.
    QColor newColor  = QColorDialog::getColor(currentColor);
    if(newColor.isValid()){
        _valueChanged(newColor.name());

        _editingFinished();
    }
}

void KeyEditWidget::pickPath()
{
    QString path = "";
    if(isFilePath){
        path = QFileDialog::getOpenFileName(this, "Select File" , oldValue.toString());
    }else{
        path = QFileDialog::getExistingDirectory(this, "Select Path" , oldValue.toString() ,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    if(path != ""){
        _valueChanged(path);
        _editingFinished();
    }
}

void KeyEditWidget::_valueChanged(QVariant value)
{
    newValue = value;
}


void KeyEditWidget::_editingFinished()
{
    if(oldValue != newValue && (!newValue.isNull() && newValue.isValid())){
        valueChanged(groupName, keyName, newValue);
        oldValue = newValue;
    }

    //UPDATE COLOR.
    if(keyType == KEY_COLOR){
        updateColorWidget(oldValue.toString());
    }else if(isPath){
        QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(valueBox);
        if(lineEdit){
            lineEdit->setText(oldValue.toString());
        }
    }
}
