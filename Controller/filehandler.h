#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <QObject>
#include <QFileDialog>
#include "enumerations.h"
class FileHandler : public QObject
{
    Q_OBJECT
public:
    FileHandler();

    static QString selectFile(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "", QString initialFile = "");
    static QStringList selectFiles(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "",  QString initialFile = "");

    static QString readTextFile(QString filePath);

    static bool writeTextFile(QString filePath, QString fileData);
    static QString writeTempTextFile(QString fileData, QString extension="");

signals:
    void notification(NOTIFICATION_TYPE type, QString notificationTitle, QString notificationText, QPair<QString, QString> notificationIcon);
public:
    static FileHandler* getFileHandler();
private:

    static void _notification(NOTIFICATION_TYPE type, QString notificationTitle, QString notificationText, QPair<QString, QString> notificationIcon);

    static bool ensureDirectory(QString path);
    static QString getTempFileName(QString suffix);
    static QString getTimestamp();

    static QFileDialog* getFileDialog();
    QFileDialog* _getFileDialog();

    QFileDialog* fileDialog;
private:
    static FileHandler* handlerSingleton;
};

#endif // FILEHANDLER_H
