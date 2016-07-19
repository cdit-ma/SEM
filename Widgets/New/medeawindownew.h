#ifndef MEDEAWINDOWNEW_H
#define MEDEAWINDOWNEW_H

#include <QMainWindow>

class MedeaWindowNew : public QMainWindow{
Q_OBJECT

public:
    MedeaWindowNew(QWidget* parent = 0);







    // QWidget interface
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *);


private:
    bool paintArrows;

    QAction* resetDockedWidgetsAction;


    // QMainWindow interface
public:
    QMenu *createPopupMenu();
};

#endif


