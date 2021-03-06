#ifndef TIMEPROGRESSBAR_H
#define TIMEPROGRESSBAR_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>

class TimeProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit TimeProgressBar(QWidget* parent = nullptr);

public slots:
    void themeChanged();

    void setTimeRange(qint64 start_time, qint64 end_time);
    void updateEndTime(qint64 time);
    void setCurrentTime(qint64 time);
    void incrementCurrentTime(int ms);

    void resetTimeProgress();

private:
    void updateProgressValue();
    void updateTotalDuration();

    qint64 getElapsedTime() const;
    qint64 getDuration() const;

    QString getDurationString(qint64 duration_ms) const;
    void leftPad(QString& str, int intended_length, char pad_char) const;

    qint64 start_time_ = 0;
    qint64 end_time_ = 0;
    qint64 current_time_ = 0;
    qint64 duration_ = 0;

    QLabel* elapsed_time_label_ = nullptr;
    QLabel* total_time_label_ = nullptr;
    QProgressBar* time_bar_ = nullptr;
};

#endif // TIMEPROGRESSBAR_H