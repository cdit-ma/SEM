#include "dataeditwidget.h"
#include <QVBoxLayout>


#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "../../View/theme.h"

#define SMALL_SQUARE 25
DataEditWidget::DataEditWidget(QString dataKey, QString label, SETTING_TYPE type, QVariant data, QWidget *parent) : QWidget(parent)
{
    this->dataKey = dataKey;
    this->label = label;
    this->currentData = data;
    this->type = type;

    setContentsMargins(0,0,0,0);
    setupLayout();
    themeChanged();
}

void DataEditWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    QPushButton* button = dynamic_cast<QPushButton*>(editWidget_2);
    if(button){
        QString style = "border:1px solid " + theme->getAltBackgroundColorHex() + ";";
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
            style += " background: " + currentData.toString() + ";";
            break;
        }
        default:{
            break;
        }

        }
        button->setStyleSheet(style);
    }
    if(editLabel){
        editLabel->setStyleSheet("color: " + Theme::theme()->getTextColorHex() + ";");
    }

    if(editWidget_1){
        QString style = "color:" + Theme::theme()->getTextColorHex() + ";";
        if(type != ST_BOOL){
            style += "background:"+ Theme::theme()->getAltBackgroundColorHex() + ";";
        }
        editWidget_1->setStyleSheet(style);
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


        editWidget_1 = lineEdit;
        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case ST_BOOL:{
        QCheckBox* checkBox = new QCheckBox(label, this);

        editWidget_1 = checkBox;
        editLayout->removeWidget(editLabel);

        editLayout->addWidget(editWidget_1, 1);
        editLabel->deleteLater();
        editLabel = 0;
        break;
    }
    case ST_INT:{
        QSpinBox* spinBox = new QSpinBox(this);



        editWidget_1 = spinBox;
        editLayout->addWidget(editWidget_1, 1);
        break;
    }
    case ST_PATH:
    case ST_FILE:{
        QLineEdit* lineEdit = new QLineEdit(this);\

        QPushButton* button = new QPushButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
        button->setFlat(true);

        editWidget_1 = lineEdit;
        editWidget_2 = button;
        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(editWidget_2);
        break;
    }
    case ST_COLOR:{
        QLineEdit* lineEdit = new QLineEdit(this);



        QPushButton* button = new QPushButton(this);
        button->setFixedSize(SMALL_SQUARE, SMALL_SQUARE);
        button->setFlat(true);

        editWidget_1 = lineEdit;
        editWidget_2 = button;

        editLayout->addWidget(editWidget_1, 1);
        editLayout->addWidget(editWidget_2);
        break;
    }
    default:{



    }
    }



}
