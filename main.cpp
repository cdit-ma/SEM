#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "medeawindow.h"
#include "modeltester.h"

#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QProcess>

#include <QList>
#include <QPair>

/*
 * Use this to override and set your own custom icon sizes in a QMenu
#include <QStyle>
#include <QProxyStyle>
class MyStyle: public QProxyStyle
{
    public:

    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const {
        int s = QProxyStyle::pixelMetric(metric, option, widget);
        if (metric == QStyle::PM_SmallIconSize) {
            s = 20;
        }
        return s;
    }
};

    MyStyle* style = new MyStyle();
    a.setStyle( style);
*/



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Actions/MEDEA.png"));
    a.setApplicationName("MEDEA");

    QFont font = QFont("Verdana");
    font.setPointSizeF(8.5);
    a.setFont(font);

    QCoreApplication::setOrganizationName("Defence Information Group");
    QCoreApplication::setApplicationName("MEDEA");
    QCoreApplication::setApplicationVersion("17");

    QString GraphMLFile = "";
    if (argc == 2) {
        GraphMLFile = QString(argv[1]);
    }



    MedeaWindow *w = new MedeaWindow(GraphMLFile);
    w->show();
    return a.exec();
}
