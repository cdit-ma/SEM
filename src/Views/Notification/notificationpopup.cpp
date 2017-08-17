#include <QApplication>
#include "notificationpopup.h"
#include "notificationobject.h"

NotificationPopup::NotificationPopup():PopupWidget(PopupWidget::TYPE::POPUP, 0) {
    setupLayout();
    timer = new QTimer(this);
    timer->setInterval(5000);

    //Hide the notification popup on timeout
    connect(timer, &QTimer::timeout, this, &QDialog::hide);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationPopup::themeChanged);
    themeChanged();
}

void NotificationPopup::DisplayNotification(NotificationObject* notification){
    timer->stop();

    auto font_metrics = label->fontMetrics();
    auto notification_text  = font_metrics.elidedText(notification->description(), Qt::ElideMiddle, 500);
    
    if(notification_text != label->text()){
        label->setText(notification_text);
    }

    auto pixmap = Theme::theme()->getImage(notification->iconPath(), notification->iconName(), QSize(32,32), getSeverityColor(notification->severity()));
    
    if (pixmap.isNull()) {
        pixmap = Theme::theme()->getImage("Icons", "circleInfo", QSize(32,32), getSeverityColor(notification->severity()));
    }

    if(!icon->pixmap() || icon->pixmap()->cacheKey() != pixmap.cacheKey()){
        icon->setPixmap(pixmap);
    }

    QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);

    timer->start();
}

void NotificationPopup::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet("QLabel{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + "; }");
}

void NotificationPopup::setupLayout(){
    widget = new QWidget(this);
    
    icon = new QLabel(this);
    icon->setFixedSize(QSize(32,32));
    
    label = new QLabel(this);
    label->setFont(QFont(font().family(), 11));
    
    widget->setContentsMargins(5, 2, 5, 2);
    
    auto layout = new QHBoxLayout(widget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(label);
    
    setWidget(widget);
}


void NotificationPopup::adjustSize(){
    QWidget::adjustSize();
}