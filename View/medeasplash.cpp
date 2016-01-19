#include "medeasplash.h"
#include <QBoxLayout>
#include <QDebug>

MedeaSplash::MedeaSplash(QString label, QString subLabel, const QPixmap &pixmap):QSplashScreen()
{
    _pixmap = pixmap.scaledToWidth(IMAGE_WIDTH, Qt::SmoothTransformation);
    _label = label;
    _subLabel = subLabel;



    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::SplashScreen);


    setupLayout();

}

void MedeaSplash::showMessage(const QString &message, int alignment, const QColor &color)
{
    qCritical() << "show MESSAGE";
    if(loadText){
        loadText->setText(message);
    }
}

void MedeaSplash::setupLayout()
{
    QVBoxLayout* mainLayout = new QVBoxLayout();
    QVBoxLayout* textLayout = new QVBoxLayout();
    QHBoxLayout* topLayout = new QHBoxLayout();

    mainLayout->addLayout(topLayout);


    mainLabel = new QLabel(_label, this);
    subLabel = new QLabel(_subLabel, this);
    loadText = new QLabel(this);
    mainLabel->setAlignment(Qt::AlignCenter);
    subLabel->setAlignment(Qt::AlignCenter);
    loadText->setAlignment(Qt::AlignCenter);
    icon = new QLabel(this);

    QFont newFont = font();

    newFont.setItalic(true);
     newFont.setPixelSize(12);
    loadText->setFont(newFont);
    newFont.setItalic(false);
    newFont.setPixelSize(16);
    newFont.setBold(true);
    subLabel->setFont(newFont);
    newFont.setPixelSize(32);
    mainLabel->setFont(newFont);

    subLabel->setStyleSheet("color:#505050;");

    icon->setPixmap(_pixmap);
    topLayout->addWidget(icon);
    textLayout->addWidget(mainLabel);
    textLayout->addWidget(subLabel);
    topLayout->addLayout(textLayout);
    mainLayout->addWidget(loadText);


    this->setLayout(mainLayout);
}

