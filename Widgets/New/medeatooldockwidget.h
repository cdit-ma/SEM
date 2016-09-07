#ifndef MEDEATOOLDOCKWIDGET_H
#define MEDEATOOLDOCKWIDGET_H
#include "medeadockwidget.h"
class MedeaToolDockWidget : public MedeaDockWidget
{
    Q_OBJECT
public:
    MedeaToolDockWidget(QString title);
    void themeChanged();
};

#endif // MEDEATOOLDOCKWIDGET_H
