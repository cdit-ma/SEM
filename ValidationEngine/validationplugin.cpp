#include "validationplugin.h"
#include <QDebug>

void ValidationPlugin::printError(int checkID, Node *node, QString errorDescription)
{
    QString errorString = "Validation[" + getName() + "]: Check #" + QString::number(checkID) + ": " + errorDescription;
    qCritical() << errorString;
    emit highlightNodeError(node, errorString);
}
