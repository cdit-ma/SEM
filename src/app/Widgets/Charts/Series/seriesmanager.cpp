#include "seriesmanager.h"


SeriesManager* SeriesManager::seriesManager = 0;
int SeriesManager::series_ID = 0;


/**
 * @brief SeriesManager::SeriesManager
 * @param parent
 */
SeriesManager::SeriesManager(QObject* parent)
    : QObject(parent)
{

}


/**
 * @brief SeriesManager::manager
 * @return
 */
SeriesManager* SeriesManager::manager()
{
    if (!seriesManager) {
        seriesManager = new SeriesManager();
    }
    return seriesManager;
}


/**
 * @brief SeriesManager::~SeriesManager
 */
SeriesManager::~SeriesManager()
{

}
