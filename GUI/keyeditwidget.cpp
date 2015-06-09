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


    vLayout = new QVBoxLayout();

    vLayout->setSpacing(0);
    vLayout->setMargin(0);
    setLayout(vLayout);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->setMargin(0);

    vLayout->addLayout(hLayout);



    QLabel* keyLabel = new QLabel(hrKeyName);

    if(description != ""){
        descriptionBox = new QTextBrowser() ;

        descriptionBox->setReadOnly(true);
        descriptionBox->setStyleSheet("font-size: 10px;color:#333;border:0px;");// background-color:#F0F0F0;");
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

    bool isFile = customType == "File";




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
    }else if(customType == "File"){
        QLineEdit* lineEdit = new QLineEdit();
        lineEdit->setEnabled(false);

        hLayout->addWidget(lineEdit);
        lineEdit->setText(stringValue);
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));
        this->oldValue = stringValue;
        valueBox = lineEdit;
    }else{
        QLineEdit* lineEdit = new QLineEdit();
        hLayout->addWidget(lineEdit);
        lineEdit->setText(stringValue);
        connect(lineEdit, SIGNAL(textChanged(QString)), this, SLOT(_valueChanged(QString)));
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(_editingFinished()));
        this->oldValue = stringValue;

        valueBox = lineEdit;
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
