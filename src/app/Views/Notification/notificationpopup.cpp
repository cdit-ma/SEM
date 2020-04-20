#include "notificationpopup.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../theme.h"

#include <QHBoxLayout>

NotificationPopup::NotificationPopup()
	: PopupWidget(PopupWidget::TYPE::SPLASH, nullptr)
{
    setupLayout();

    timer = new QTimer(this);
    timer->setInterval(5000);

    // Hide the notification popup on timeout
    connect(timer, &QTimer::timeout, this, &NotificationPopup::Hide);

    installEventFilter(this);
    setAttribute(Qt::WA_ShowWithoutActivating);

    connect(Theme::theme(), &Theme::theme_Changed, this, &NotificationPopup::themeChanged);
    themeChanged();
}

void NotificationPopup::Hide()
{
    hide();
    timer->stop();
    current_notification.reset();
}

void NotificationPopup::DisplayNotification(QSharedPointer<NotificationObject> notification)
{
    current_notification = notification;

    auto font_metrics = label->fontMetrics();
    auto notification_text  = font_metrics.elidedText(notification->getTitle(), Qt::ElideMiddle, 500);
    if (notification_text != label->text()) {
        label->setText(notification_text);
    }

    auto icon_size = icon->size();
    if (notification->getSeverity() == Notification::Severity::RUNNING) {
        auto movie = Theme::theme()->getGif("Icons", "loading");
        icon->setMovie(movie);
    } else {
        auto notification_icon = notification->getIcon();
        auto icon_color = Theme::theme()->getSeverityColor(notification->getSeverity());
        auto pixmap = Theme::theme()->getImage(notification_icon.first, notification_icon.second, icon_size, icon_color);
        if (pixmap.isNull()) {
            pixmap = Theme::theme()->getImage("Icons", "circleInfo", icon_size, icon_color);
        }
        if (!icon->pixmap() || icon->pixmap()->cacheKey() != pixmap.cacheKey()) {
            icon->setPixmap(pixmap);
        }
    }

    // TODO: Investigate why adjustSize() is not being called directly
    QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
    timer->start();
}

void NotificationPopup::themeChanged()
{
    auto theme = Theme::theme();
    icon->setFixedSize(theme->getLargeIconSize());
    label->setFont(theme->getLargeFont());
    label->setStyleSheet(theme->getLabelStyleSheet());
}

void NotificationPopup::setupLayout()
{
    icon = new QLabel(this);
    icon->setScaledContents(true);
    icon->setAlignment(Qt::AlignCenter);

    label = new QLabel(this);
    
    widget = new QWidget(this);
    widget->setContentsMargins(5, 2, 5, 2);
    
    auto layout = new QHBoxLayout(widget);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(icon);
    layout->addWidget(label);

    widget = new QWidget(this);
    widget->setContentsMargins(5, 2, 5, 2);
    widget->setLayout(layout);
    setWidget(widget);
}

bool NotificationPopup::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::MouseButtonPress) {
        Hide();
    }
    return false;
}