#ifndef PLAYBACKCONTROLSWIDGET_H
#define PLAYBACKCONTROLSWIDGET_H

#include "timeprogressbar.h"

#include <QWidget>
#include <QToolBar>
#include <QAction>

class PlaybackControlsWidget : public QWidget
{
    Q_OBJECT

public:
    PlaybackControlsWidget(QWidget* parent = nullptr);

signals:
    void play();
    void pause();
    void jumpToNextActivity();
    void jumpToPreviousActivity();
    void jumpToStart();
    void jumpToEnd();

public slots:
    void themeChanged();

    void setControlsEnabled(bool enabled);
    void setPlayPauseCheckedState(bool checked);

    void setTimeRange(qint64 start_time, qint64 end_time);
    void updateEndTime(qint64 time);
    void setCurrentTime(qint64 time);
    void incrementCurrentTime(int ms);

    void resetTimeProgress();

private:
    TimeProgressBar time_progressbar_;

    QToolBar* controls_toolbar_ = nullptr;
    QAction* play_pause_action_ = nullptr;
    QAction* jump_to_next_action_ = nullptr;
    QAction* jump_to_prev_action_ = nullptr;
    QAction* jump_to_start_action_ = nullptr;
    QAction* jump_to_end_action_ = nullptr;
};

#endif // PLAYBACKCONTROLSWIDGET_H
