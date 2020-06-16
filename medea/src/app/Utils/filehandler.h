#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QFileDialog>

#include "../Controllers/NotificationManager/notificationmanager.h"

class FileHandler : public QObject
{
    Q_OBJECT
    
public:
    FileHandler();

    static QString selectFile(QWidget* parent, const QString& windowTitle, QFileDialog::FileMode fileMode, bool write, const QString& nameFilter = "", const QString& defaultSuffix = "", const QString& initialFile = "");
    static QStringList selectFiles(QWidget* parent, const QString& windowTitle, QFileDialog::FileMode fileMode, bool write, const QString& nameFilter = "", const QString& defaultSuffix = "",  const QString& initialFile = "");
    
    static bool isFileReadable(const QString& filePath);
    static QString readTextFile(const QString& filePath);
    
    static bool writeTextFile(const QString& filePath, const QString& fileData, bool notify = true);
    static QString writeTempTextFile(const QString& fileData, const QString& extension="");
    static QString getTempFileName(const QString& suffix, bool timestamp = true);
    
    static void sanitizeFilePath(QString &filePath);

    static bool ensureDirectory(const QString& path);
    static bool removeDirectory(const QString& path);
    static bool removeFile(const QString& path);
    
    static bool isAutosaveFilePath(const QString& path);
    static QString getAutosaveFilePath(const QString& path);
    static QString getFileFromAutosavePath(QString autosave_path);

    static FileHandler* getFileHandler();
    
private:
    static bool _writeTextFile(const QString& filePath, const QString& fileData, bool notify);
    static void _notification(Notification::Severity severity, const QString& notificationText, const QString& iconPath, const QString& iconName, bool toast = false);

    static QString getTimestamp();
    
    static QFileDialog* getFileDialog();
    QFileDialog* _getFileDialog();

    QFileDialog* fileDialog = nullptr;
};

#endif // FILEHANDLER_H
