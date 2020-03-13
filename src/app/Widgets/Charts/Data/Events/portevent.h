#ifndef PORTEVENT_H
#define PORTEVENT_H

#include "event.h"

class PortEvent : public MEDEA::Event
{
    Q_OBJECT

public:
    enum PortEventType {
        UNKNOWN,
        SENT,
        RECEIVED,
        STARTED_FUNC,
        FINISHED_FUNC,
        IGNORED,
        EXCEPTION,
        MESSAGE
    };

    explicit PortEvent(const AggServerResponse::Port& port,
                       quint64 sequence_num,
                       PortEventType type,
                       QString message,
                       qint64 time,
                       QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const override;

    const QString& getSeriesID() const override;
    const QString& getID() const override;

    PortEventType getType() const;
    static const QString& getTypeString(PortEventType type);

private:
    PortEventType type_;

    quint64 sequence_num_;

    // The series_id_ is formed by what is required to group events together into a series
    QString series_id_;
    QString id_;
    QString message_;
};

#endif // PORTEVENT_H