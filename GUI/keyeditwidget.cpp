#include "keyeditwidget.h"
#include "appsettings.h"

#include <QHBoxLayout>
#include <QVariant>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include <QComboBox>
#include <QSpinBox>

KeyEditWidget::KeyEditWidget(AppSettings *parent, QString g, QString k, QVariant v)
{
    groupName = g;
    keyName = k;
    hrKeyName = parent->getReadableValue(k);

    newValue ="";


    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setMargin(0);
    setLayout(hLayout);


    QLabel* keyLabel = new QLabel(hrKeyName);
    hLayout->addWidget(keyLabel);

    labelBox = keyLabel;


    keyLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);




    //hLayout->setStretch(0,-1);
    //hLayout->setStretch(1,1);

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
        QCheckBox* checkBox = new QCheckBox();
        checkBox->setChecked(boolValue);
        connect(checkBox,SIGNAL(clicked(bool)), this, SLOT(_boolChanged(bool)));
        connect(checkBox, SIGNAL(clicked()), this, SLOT(_editingFinished()));
        hLayout->addWidget(checkBox);
        valueBox = checkBox;


    }else if (isInt){
        QSpinBox* intEdit = new QSpinBox(0);
        intEdit->setRange(0,4000);

        hLayout->addWidget(intEdit);
        intEdit->setValue(intValue);
        connect(intEdit, SIGNAL(valueChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(intEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));

        this->oldValue = QString::number(intValue);

        valueBox = intEdit;
    }else if(stringValue != ""){
        QLineEdit* lineEdit = new QLineEdit();
        hLayout->addWidget(lineEdit);
        lineEdit->setText(stringValue);
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));
        this->oldValue = stringValue;

        valueBox = lineEdit;
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
