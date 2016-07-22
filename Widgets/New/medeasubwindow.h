#ifndef MEDEASUBWINDOW_H
#define MEDEASUBWINDOW_H

#include "medeaviewwindow.h"
class MedeaSubWindow : public MedeaViewWindow
{
    Q_OBJECT
    friend class MedeaWindowManager;
public:
    MedeaSubWindow();
};

#endif // MEDEASUBWINDOW_H
