#include "keyeditwidget.h"

#include <QVariant>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>

KeyEditWidget::KeyEditWidget(QObject *parent, QString g, QString k, QVariant v)
{
    groupName = g;
    keyName = k;
    hrKeyName = keyName;

    hrKeyName = hrKeyName.replace("_", " ");



    horizontalLayout = new QHBoxLayout();


    QLabel* keyLabel = new QLabel(hrKeyName);
    horizontalLayout->addWidget(keyLabel);


    keyLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

    horizontalLayout->setStretch(0,1);

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
    }

    if(stringListValue.size() == 1){
        isStringList = false;
    }



    if(isBool){
        QCheckBox* checkBox = new QCheckBox();
        checkBox->setChecked(boolValue);
        connect(checkBox,SIGNAL(clicked(bool)), this, SLOT(_boolChanged(bool)));
        horizontalLayout->addWidget(checkBox);
    }else if (isInt){
        QSpinBox* intEdit = new QSpinBox(0);
        intEdit->setRange(0,4000);

        horizontalLayout->addWidget(intEdit);
        intEdit->setValue(intValue);
        connect(intEdit, SIGNAL(valueChanged(QString)), this, SLOT(_valueChanged(QString)));
    }else if(isStringList){
        QComboBox* stringlistEdit = new QComboBox();
        stringlistEdit->addItems(stringListValue);
        horizontalLayout->addWidget(stringlistEdit);

    }else if(stringValue != ""){
        QLineEdit* lineEdit = new QLineEdit();
        horizontalLayout->addWidget(lineEdit);
        lineEdit->setText(stringValue);
    }
}

QHBoxLayout* KeyEditWidget::getLayout()
{
    return horizontalLayout;
}

void KeyEditWidget::_boolChanged(bool value)
{
    newValue = "false";
    if(value){
        newValue = "true";
    }
    valueChanged(groupName, keyName, newValue);
}

void KeyEditWidget::_valueChanged(QString value)
{
    newValue = value;
    valueChanged(groupName, keyName, newValue);
}
