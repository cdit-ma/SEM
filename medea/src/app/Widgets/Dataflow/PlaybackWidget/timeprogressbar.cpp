#include "timeprogressbar.h"
#include "../../../theme.h"

#include <QHBoxLayout>
#include <QDateTime>

/**
 * @brief TimeProgressBar::TimeProgressBar
 * @param parent
 */
TimeProgressBar::TimeProgressBar(QWidget* parent)
    : QWidget(parent)
{
    elapsed_time_label_ = new QLabel("00:20", this);
    total_time_label_ = new QLabel("03:14", this);

    time_bar_ = new QProgressBar(this);
    time_bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    time_bar_->setTextVisible(false);
    time_bar_->setRange(0, 100);

    auto layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->addWidget(elapsed_time_label_);
    layout->addWidget(time_bar_);
    layout->addWidget(total_time_label_);

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimeProgressBar::themeChanged);
    themeChanged();
}

/**
 * @brief TimeProgressBar::themeChanged
 */
void TimeProgressBar::themeChanged()
{
    Theme* theme = Theme::theme();
    elapsed_time_label_->setStyleSheet(theme->getLabelStyleSheet() + "QLabel:disabled{ color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}");
    total_time_label_->setStyleSheet(theme->getLabelStyleSheet() + "QLabel:disabled{ color:" + theme->getTextColorHex(ColorRole::DISABLED) + ";}");
    time_bar_->setStyleSheet(theme->getProgressBarStyleSheet() + "QProgressBar{ margin: 5px; }");
}

/**
 * @brief TimeProgressBar::setTimeRange
 * @param start_time
 * @param end_time
 */
void TimeProgressBar::setTimeRange(qint64 start_time, qint64 end_time)
{
    if (start_time > end_time) {
        end_time = start_time;
    }
    start_time_ = start_time;
    end_time_ = end_time;
    updateTotalDuration();
    resetTimeProgress();
}

/**
 * @brief TimeProgressBar::updateEndTime
 * @param time
 * @throws std::invalid_argument
 */
void TimeProgressBar::updateEndTime(qint64 time)
{
    if (time < start_time_) {
        throw std::invalid_argument("TimeProgressBar::updateEndTime - The provided end time is before the start time");
    }
    end_time_ = time;
    updateTotalDuration();
}

/**
 * @brief TimeProgressBar::setCurrentTime
 * @param time
 */
void TimeProgressBar::setCurrentTime(qint64 time)
{
    if (time <= start_time_) {
        resetTimeProgress();
    } else {
        if (time > end_time_) {
            time = end_time_;
        }
        current_time_ = time;
        elapsed_time_label_->setText(getDurationString(current_time_ - start_time_));
        updateProgressValue();
    }
}

/**
 * @brief TimeProgressBar::incrementCurrentTime
 * @param ms
 */
void TimeProgressBar::incrementCurrentTime(int ms)
{
    auto&& incremented_time = current_time_ + ms;
    if (incremented_time > end_time_) {
        setCurrentTime(end_time_);
    } else {
        setCurrentTime(incremented_time);
    }
}

/**
 * @brief TimeProgressBar::resetTimeProgress
 */
void TimeProgressBar::resetTimeProgress()
{
    current_time_ = start_time_;
    elapsed_time_label_->setText("00.000");
    time_bar_->reset();
}

/**
 * @brief TimeProgressBar::updateProgressValue
 */
void TimeProgressBar::updateProgressValue()
{
    if (duration_ > 0) {
        auto&& elapsed_time = current_time_ - start_time_;
        double ratio = elapsed_time / static_cast<double>(duration_);
        time_bar_->setValue(static_cast<int>(ratio * 100));
    }
}

/**
 * @brief TimeProgressBar::updateTotalDuration
 */
void TimeProgressBar::updateTotalDuration()
{
    duration_ = end_time_ - start_time_;
    total_time_label_->setText(getDurationString(duration_));
    updateProgressValue();
}

/**
 * @brief TimeProgressBar::getElapsedTime
 * @return
 */
qint64 TimeProgressBar::getElapsedTime() const
{
    return current_time_ - start_time_;
}

/**
 * @brief TimeProgressBar::getDuration
 * @return
 */
qint64 TimeProgressBar::getDuration() const
{
    return end_time_ - start_time_;
}

/**
 * @brief TimeProgressBar::getDurationString
 * This returns a string representation of the provided time duration in the following display format - "00:00:00.000"
 * @param duration_ms
 * @return
 */
QString TimeProgressBar::getDurationString(qint64 duration_ms) const
{
    auto hours = duration_ms / 3.6e6;
    auto mins = duration_ms / 6e4;
    auto secs = duration_ms / 1e3;
    auto msecs = duration_ms;

    // This variable is used to check if the proceeding time strings need to be appended to time_str
    bool append = false;
    QString time_str;

    if (hours >= 1) {
        auto&& h = static_cast<int>(hours);
        time_str = QString::number(h);
        mins -= h * 60;
        append = true;
    }

    if (mins >= 1) {
        auto&& m = static_cast<int>(mins);
        auto&& m_str = QString::number(m);
        leftPad(m_str, 2, '0');
        if (append) {
            time_str += ":" + m_str;
        } else {
            time_str = m_str;
        }
        secs -= m * 60;
        append = true;
    }

    {
        auto&& s = static_cast<int>(secs);
        auto&& s_str = QString::number(s);
        leftPad(s_str, 2, '0');
        if (append) {
            time_str += ":" + s_str;
        } else {
            time_str = s_str;
        }
        msecs -= s * 1000;
    }

    auto&& ms = static_cast<int>(msecs);
    auto&& ms_str = QString::number(ms);
    leftPad(ms_str, 3, '0');
    time_str += "." + ms_str;

    return time_str;
}

/**
 * @brief TimeProgressBar::leftPad
 * @param str
 * @param intended_length
 * @param pad_char
 */
void TimeProgressBar::leftPad(QString& str, int intended_length, char pad_char) const
{
    while (str.length() < intended_length) {
        str = pad_char + str;
    }
}