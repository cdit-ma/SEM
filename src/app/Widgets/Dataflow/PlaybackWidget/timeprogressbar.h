#ifndef TIMEPROGRESSBAR_H
#define TIMEPROGRESSBAR_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>

class TimeProgressBar : public QWidget
{
    Q_OBJECT

public:
    TimeProgressBar(QWidget* parent = nullptr);

public slots:
    void themeChanged();

    void setTimeRange(qint64 start_time, qint64 end_time);
    void updateEndTime(qint64 time);
    void setCurrentTime(qint64 time);
    void incrementCurrentTime(int ms);

    void resetProgress();

private:
    void updateProgressValue();
    void updateTotalDuration();

    QString getDurationString(qint64 duration_ms) const;

    qint64 start_time_;
    qint64 end_time_;
    qint64 current_time_;
    qint64 duration_;

    QLabel* elapsed_time_label_ = nullptr;
    QLabel* total_time_label_ = nullptr;
    QProgressBar* time_bar_ = nullptr;
};

#endif // TIMEPROGRESSBAR_H
