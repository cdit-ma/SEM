#ifndef SERIESMANAGER_H
#define SERIESMANAGER_H

#include <QObject>

class SeriesManager : public QObject
{
    Q_OBJECT

public:
    explicit SeriesManager(QObject* parent = 0);
    static SeriesManager* manager();
    ~SeriesManager();

private:
    static SeriesManager* seriesManager;
    static int series_ID;

};

#endif // SERIESMANAGER_H
