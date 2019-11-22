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
                       const QString& message,
                       qint64 time,
                       QObject* parent = nullptr);

    QString toString(const QString& dateTimeFormat) const;

    const QString& getID() const;
    const AggServerResponse::Port& getPort() const;

    PortEventType getType() const;
    static const QString& getTypeString(PortEventType type);

private:    
    const AggServerResponse::Port port_;
    const PortEventType type_;
    const quint64 sequence_num_;
    const QString message_;
};

#endif // PORTEVENT_H
