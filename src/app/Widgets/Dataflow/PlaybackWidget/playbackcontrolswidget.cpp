#include "playbackcontrolswidget.h"
#include "../../../theme.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

/**
 * @brief PlaybackControlsWidget::PlaybackControlsWidget
 * @param parent
 */
PlaybackControlsWidget::PlaybackControlsWidget(QWidget* parent)
    : QWidget(parent),
      time_progressbar_(this)
{
    controls_toolbar_ = new QToolBar(this);
    controls_toolbar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    auto toolbar_layout = new QHBoxLayout;
    toolbar_layout->setMargin(0);
    toolbar_layout->setSpacing(5);
    toolbar_layout->setContentsMargins(0, 0, 0, 0);
    toolbar_layout->addWidget(controls_toolbar_);
    toolbar_layout->addWidget(&time_progressbar_, 1);

    auto main_layout = new QVBoxLayout(this);
    main_layout->setMargin(0);
    main_layout->setContentsMargins(0, 0, 5, 0);
    main_layout->addLayout(toolbar_layout);

    jump_to_start_action_ = controls_toolbar_->addAction("Jump To Start");
    jump_to_prev_action_ = controls_toolbar_->addAction("Jump To Previous Activity");
    play_pause_action_ = controls_toolbar_->addAction("Start/Pause Playback");
    jump_to_next_action_ = controls_toolbar_->addAction("Jump To Next Activity");
    jump_to_end_action_ = controls_toolbar_->addAction("Jump To End");

    play_pause_action_->setCheckable(true);
    connect(play_pause_action_, &QAction::triggered, [this] (bool checked)
    {
        if (checked) {
            emit play();
        } else {
            emit pause();
        }
    });

    connect(jump_to_start_action_, &QAction::triggered, this, &PlaybackControlsWidget::jumpToStart);
    connect(jump_to_prev_action_, &QAction::triggered, this, &PlaybackControlsWidget::jumpToPreviousActivity);
    connect(jump_to_next_action_, &QAction::triggered, this, &PlaybackControlsWidget::jumpToNextActivity);
    connect(jump_to_end_action_, &QAction::triggered, this, &PlaybackControlsWidget::jumpToEnd);

    connect(Theme::theme(), &Theme::theme_Changed, this, &PlaybackControlsWidget::themeChanged);
    themeChanged();
}


/**
 * @brief PlaybackControlsWidget::themeChanged
 */
void PlaybackControlsWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    // NOTE: Need to use stretcher widgets if we decide to use the whole widget as the progress bar

    controls_toolbar_->setIconSize(theme->getIconSize() * 1.5);
    controls_toolbar_->setStyleSheet(theme->getToolBarStyleSheet() +
                                     "QToolBar {" +
                                     "padding: 0px;" +
                                     "background:" + theme->getBackgroundColorHex() + ";}" +
                                     "QToolButton{ border: none; background: transparent; }" +
                                     "QToolButton::checked:!hover{ background: transparent; }");

    QIcon playPauseIcon;
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPlay", QSize(), theme->getMenuIconColor()));
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPause", QSize(), theme->getMenuIconColor()), QIcon::Normal, QIcon::On);
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPlay", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active);
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPause", QSize(), theme->getMenuIconColor(ColorRole::SELECTED)), QIcon::Active, QIcon::On);
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPause", QSize(), theme->getMenuIconColor(ColorRole::DISABLED)), QIcon::Disabled, QIcon::On);
    playPauseIcon.addPixmap(theme->getImage("Icons", "avPause", QSize(), theme->getMenuIconColor(ColorRole::DISABLED)), QIcon::Disabled, QIcon::Off);

    play_pause_action_->setIcon(playPauseIcon);
    jump_to_start_action_->setIcon(theme->getIcon("Icons", "skipPrevious"));
    jump_to_prev_action_->setIcon(theme->getIcon("Icons", "fastRewind"));
    jump_to_next_action_->setIcon(theme->getIcon("Icons", "fastForward"));
    jump_to_end_action_->setIcon(theme->getIcon("Icons", "skipNext"));
}


/**
 * @brief PlaybackControlsWidget::setControlsEnabled
 * @param enabled
 */
void PlaybackControlsWidget::setControlsEnabled(bool enabled)
{
    if (!enabled) {
        setPlayPauseCheckedState(false);
    }
    controls_toolbar_->setEnabled(enabled);
    time_progressbar_.setEnabled(enabled);
}


/**
 * @brief PlaybackControlsWidget::setPlayPauseCheckedState
 * @param checked
 */
void PlaybackControlsWidget::setPlayPauseCheckedState(bool checked)
{
    play_pause_action_->setChecked(checked);
}


/**
 * @brief PlaybackControlsWidget::setTimeRange
 * @param start_time
 * @param end_time
 */
void PlaybackControlsWidget::setTimeRange(qint64 start_time, qint64 end_time)
{
    time_progressbar_.setTimeRange(start_time, end_time);
}


/**
 * @brief PlaybackControlsWidget::updateEndTime
 * @param time
 */
void PlaybackControlsWidget::updateEndTime(qint64 time)
{
    time_progressbar_.updateEndTime(time);
}


/**
 * @brief PlaybackControlsWidget::setCurrentTime
 * @param time
 */
void PlaybackControlsWidget::setCurrentTime(qint64 time)
{
    time_progressbar_.setCurrentTime(time);
}


/**
 * @brief PlaybackControlsWidget::incrementCurrentTime
 * @param ms
 */
void PlaybackControlsWidget::incrementCurrentTime(int ms)
{
    time_progressbar_.incrementCurrentTime(ms);
}


/**
 * @brief PlaybackControlsWidget::resetTimeProgress
 */
void PlaybackControlsWidget::resetTimeProgress()
{
    time_progressbar_.resetTimeProgress();
}
