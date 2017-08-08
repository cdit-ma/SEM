//Auto Generated File
#include "version.h"
#include "cmakevars.h"

QString APP_URL(){
    #ifdef MEDEA_URL
        return MEDEA_URL "/";
    #else
        return "?";
    #endif
}

QString APP_VERSION(){
    #ifdef MEDEA_VERSION
        return MEDEA_VERSION;
    #else
        return "?";
    #endif
}