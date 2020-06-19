//Auto Generated File
#include "version.h"
#include "sem_version.hpp"

namespace Version {
QString GetMedeaUrl(){
    return QString::fromStdString(std::string(SEM::GetUrl()));
}

QString GetMedeaVersion(){
    // XXX: Eugh
    return QString::fromStdString(std::string(SEM::GetVersion()));
}
}
