#ifndef MEDEASPLASH_H
#define MEDEASPLASH_H
#include <QSplashScreen>
#include <QLabel>

#define IMAGE_WIDTH 100
class MedeaSplash : public QSplashScreen
{
    Q_OBJECT
public:
    MedeaSplash(QString label, QString subLabel, const QPixmap & pixmap);

public slots:
    void showMessage(const QString & message, int alignment = Qt::AlignLeft, const QColor & color = Qt::black);
private:
    void setupLayout();
    QLabel *mainLabel;
    QLabel *subLabel;
    QLabel *loadText;
    QLabel *icon;
    QPixmap *image;

    QString _label;
    QString _subLabel;
    QPixmap _pixmap;
};

#endif // MEDEASPLASH_H
