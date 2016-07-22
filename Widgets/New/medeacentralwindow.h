#ifndef MEDEACENTRALWINDOW_H
#define MEDEACENTRALWINDOW_H

#include "medeaviewwindow.h"
#include "medeawindowmanager.h"
class MedeaCentralWindow : public MedeaViewWindow
{
    Q_OBJECT
    friend class MedeaWindowManager;
protected:
    MedeaCentralWindow();
};

#endif // MEDEACENTRALWINDOW_H
