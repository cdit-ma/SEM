//
// Created by Cathlyn on 14/01/2020.
//

#include "../../Events/memoryutilisationevent.h"
#include "../../Events/networkutilisationevent.h"
#include "../networkutilisationeventseries.h"
#include <QDateTime>

#include "gtest/gtest.h"

// Returns the current time in MS since epoch
qint64 getCurrentTimeMS()
{
    return QDateTime::currentMSecsSinceEpoch();
}



// Required?
TEST(Basic, Constructor)
{
    auto hostname = "TestNode";
    NetworkUtilisationEventSeries series(hostname);
}

// Test for adding an event of the wrong type into the series
TEST(Basic, AddIncompatibleEvent)
{
    auto hostname = "TestNode";
    auto memory_utilisation_event = new MemoryUtilisationEvent(hostname, 25.0, getCurrentTimeMS());

    //EXPECT_THROW();

}

