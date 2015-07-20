#include "keyeditwidget.h"

#include <QHBoxLayout>
#include <QVariant>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>
#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>
#include <QPushButton>
#include <QEventLoop>

KeyEditWidget::KeyEditWidget(QString g, QString k, QString keyNameHR, QVariant v, QString description, QString customType)
{
    groupName = g;
    keyName = k;
    hrKeyName = keyNameHR;
    descriptionBox = 0;
    difference = 0;
    newValue ="";
    highlighted = false;

    hover = false;

    normalPal = palette();
    hoverPal = palette();
    hoverPal.setColor(QPalette::Background, QColor(249,249,249));


    setAutoFillBackground(true);

    vLayout = new QVBoxLayout();



    vLayout->setSpacing(0);
    vLayout->setMargin(0);
    setLayout(vLayout);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    vLayout->addLayout(hLayout);


    QPushButton* keyLabel = new QPushButton(hrKeyName);
    keyLabel->setFlat(true);
    labelStyleSheet = "QPushButton{background-color: rgba(0,0,0,0); border: 0px; text-align: left; padding-right:2px;}";//text-align:right;";
    keyLabel->setStyleSheet(labelStyleSheet);

    if(description != ""){
        descriptionBox = new QTextBrowser() ;

        descriptionBox->setReadOnly(true);
        descriptionBox->setStyleSheet("QTextBrowser{font-size: 10px;color:#333;border:0px;}");// background-color:#F0F0F0;");
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

    labelBox = keyLabel;

    bool isInt;

    bool isStringList = v.canConvert(QVariant::StringList);
    QStringList stringListValue = v.toStringList();
    double intValue = v.toInt(&isInt);
    QString stringValue = v.toString();
    bool isBool = false;
    bool boolValue;
    if(stringValue == "false" || stringValue == "true"){
        isBool = true;
        if(stringValue == "false"){
            boolValue = false;
        }else{
            boolValue = true;
        }
        this->oldValue = stringValue;
    }

    if(stringListValue.size() == 1){
        isStringList = false;
    }



    if(isBool){
        //Set the label as stretched.
        hLayout->setStretch(0,1);

        QCheckBox* checkBox = new QCheckBox();
        connect(keyLabel, SIGNAL(clicked()), checkBox, SLOT(click()));

        checkBox->setChecked(boolValue);
        connect(checkBox,SIGNAL(clicked(bool)), this, SLOT(_boolChanged(bool)));
        connect(checkBox, SIGNAL(clicked()), this, SLOT(_editingFinished()));
        hLayout->addWidget(checkBox);//,1);
        valueBox = checkBox;

        keyType = KEY_BOOL;


    }else if (isInt){
        QSpinBox* intEdit = new QSpinBox(0);
        intEdit->setRange(0,4000);

        hLayout->addWidget(intEdit,1);
        intEdit->setValue(intValue);
        connect(intEdit, SIGNAL(valueChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(intEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));

        this->oldValue = QString::number(intValue);

        valueBox = intEdit;
        keyType = KEY_INT;
    }else if(customType == "File"){
        QLineEdit* lineEdit = new QLineEdit();
        lineEdit->setEnabled(false);

        hLayout->addWidget(lineEdit,1);
        lineEdit->setText(stringValue);
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));
        this->oldValue = stringValue;
        valueBox = lineEdit;
        keyType = KEY_FILE;
    }else{
        QLineEdit* lineEdit = new QLineEdit();
        hLayout->addWidget(lineEdit,1);
        lineEdit->setText(stringValue);
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));
        this->oldValue = stringValue;

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

QString KeyEditWidget::getValue()
{
    return oldValue;
}

void KeyEditWidget::setLabelWidth(int labelWidth)
{
    if(labelBox){
        labelBox->setMinimumWidth(labelWidth+2);
    }

}

void KeyEditWidget::setValue(QVariant value)
{
    QString stringValue = value.toString();

    if(keyType == KEY_INT){
        bool isInt = false;
        int intValue = value.toInt(&isInt);

        if(isInt){
            QSpinBox* intEdit = dynamic_cast<QSpinBox*>(valueBox);
            if(intEdit){
                intEdit->setValue(intValue);
            }
        }
    }else if(keyType == KEY_BOOL){
        bool changeValue = false;
        bool boolValue = false;

        if(stringValue == "false" || stringValue == "true"){
            changeValue = true;
            if(stringValue == "true"){
                boolValue = true;
            }
        }
        if(changeValue){
            QCheckBox* boolEdit = dynamic_cast<QCheckBox*>(valueBox);
            if(boolEdit){
                boolEdit->setChecked(boolValue);
            }
        }
    }else if(keyType == KEY_STRING || keyType == KEY_FILE){
        QLineEdit* stringEdit = dynamic_cast<QLineEdit*>(valueBox);
        if(stringEdit){
            stringEdit->setText(stringValue);
        }
    }

    //Update the value
    oldValue = stringValue;
}

void KeyEditWidget::setHighlighted(bool highlighted)
{
    if(labelBox){
        if(this->highlighted != highlighted){
            this->highlighted = highlighted;
            if(highlighted){
                labelBox->setStyleSheet(labelStyleSheet + "color : blue;");
            }else{
                labelBox->setStyleSheet(labelStyleSheet);
            }
            update();
        }
    }
}


void KeyEditWidget::_boolChanged(bool value)
{
    newValue = "false";
    if(value){
        newValue = "true";
    }
}

void KeyEditWidget::_valueChanged(QString value)
{
    newValue = value;
}

void KeyEditWidget::_editingFinished()
{
    if(oldValue != newValue && (newValue != "")){
        valueChanged(groupName, keyName, newValue);
        oldValue = newValue;
    }
}

void KeyEditWidget::updatePallete()
{
    if(hover){
        setPalette(hoverPal);
    }else{
        setPalette(normalPal);
    }
}

void KeyEditWidget::enterEvent(QEvent *)
{
    hover = true;
    updatePallete();

}

void KeyEditWidget::leaveEvent(QEvent *)
{
    hover = false;
    updatePallete();
}
