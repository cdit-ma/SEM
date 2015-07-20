#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "medeawindow.h"
#include "modeltester.h"

#include <QString>
#include <QRegExp>


int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Resources/Icons/medea.png"));
    a.setApplicationName("MEDEA");

    QFont font = QFont("Verdana");
    font.setPointSizeF(8.5);
    a.setFont(font);

    QCoreApplication::setOrganizationName("Defence Information Group");
    QCoreApplication::setApplicationName("MEDEA");

    QString GraphMLFile = "";
    if (argc == 2) {
        GraphMLFile = QString(argv[1]);
    }


    MedeaWindow *w = new MedeaWindow(GraphMLFile);
    w->show();
    return a.exec();
}
