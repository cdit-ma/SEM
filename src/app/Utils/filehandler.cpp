#include "filehandler.h"

#include <QDateTime>
#include <QTextStream>
#include <QStringBuilder>
#include <QDebug>
#include "../Controllers/WindowManager/windowmanager.h"
#include "../Controllers/SettingsController/settingscontroller.h"
#include "../theme.h"
#include <QApplication>

FileHandler* handler = 0;

FileHandler::FileHandler():QObject()
{
    fileDialog = new QFileDialog(0);
    fileDialog->setModal(true);
    fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
    //Get Path.
    QString directory = SettingsController::settings()->getSetting(SETTINGS::GENERAL_MODEL_PATH).toString();
    fileDialog->setDirectory(directory);

}


QString FileHandler::selectFile(QWidget* parent, QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter, QString defaultSuffix, QString initialFile)
{
    QString file;
    QStringList files = selectFiles(parent, windowTitle, fileMode, write, nameFilter, defaultSuffix, initialFile);
    if(files.length() == 1){
        file = files.at(0);
    }
    return file;
}

QStringList FileHandler::selectFiles(QWidget* parent, QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter, QString defaultSuffix, QString initialFile)
{
    QStringList files;
    QFileDialog* fd = getFileDialog();

    fd->setWindowTitle(windowTitle);
    fd->setNameFilter(nameFilter);
    fd->setFileMode(fileMode);
    fd->setConfirmOverwrite(write);
    fd->selectFile(initialFile);

    if(write){
        fd->setAcceptMode(QFileDialog::AcceptSave);
    }else{
        fd->setAcceptMode(QFileDialog::AcceptOpen);
    }

    WindowManager::MoveWidget(fd);
    //fd->setParent(parent);
    fd->setOption(QFileDialog::ShowDirsOnly, fileMode == QFileDialog::Directory);
    if(fd->exec()){
        foreach(QString file, fd->selectedFiles()){
            if(!file.endsWith(defaultSuffix)){
                file.append(defaultSuffix);
            }
            if(!files.contains(file)){
                files.append(file);
            }
        }
    }
    return files;
}

QString FileHandler::readTextFile(QString filePath)
{
    QString fileData = "";
    QFile file(filePath);
    QFileInfo fileInfo(file);

    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QTextStream fileStream(&file);
            fileData = fileStream.readAll();
            file.close();
        }else{
            _notification(Notification::Severity::ERROR, "File '" % fileInfo.absoluteFilePath() % "' cannot be read", "Icons", "file", true);
        }
    }else{
        _notification(Notification::Severity::ERROR, "File '" % fileInfo.absoluteFilePath() % "' doesn't exist to be read", "Icons", "file", true);
    }
    return fileData;
}

bool FileHandler::isFileReadable(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    return fileInfo.exists() && fileInfo.isReadable();
}

QString FileHandler::writeTempTextFile(QString fileData, QString extension)
{
    auto path = getTempFileName(extension);

    if(!_writeTextFile(path, fileData, false)){
        path = "";
    }
    return path;
}

bool FileHandler::_writeTextFile(QString filePath, QString fileData, bool notify){
    QFile file(filePath);
    QFileInfo fileInfo(file);
    if(ensureDirectory(filePath)){
        if(file.open(QFile::WriteOnly | QFile::Text)){
            //Create stream to write the data.
            QTextStream out(&file);
            out << fileData;
            file.close();
        }else{
            _notification(Notification::Severity::ERROR, "File '" % fileInfo.absoluteFilePath() % "' cannot be written due to permissions", "Icons", "floppyDisk", true);
            return false;
        }
    }else{
        return false;
    }
    if(notify){
        _notification(Notification::Severity::SUCCESS, "File '" % fileInfo.absoluteFilePath() % "' written", "Icons", "floppyDisk");
    }
    return true;
}

bool FileHandler::writeTextFile(QString filePath, QString fileData, bool notify)
{
    return _writeTextFile(filePath, fileData, notify);
}

bool FileHandler::ensureDirectory(QString path)
{
    QFile file(path);
    QFileInfo fileInfo(file);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if(!dir.mkpath(".")){
            _notification(Notification::Severity::ERROR, "Directory '" % dir.absolutePath() % "' cannot be constructed", "Icons", "folder", true);
            return false;
        }
    }
    return true;
}

bool FileHandler::removeFile(QString path){
    QFile file(path);
    bool success = file.exists() ? QFile::remove(path) : true;

    if(!success){
        _notification(Notification::Severity::ERROR, "File '" % path % "' failed to be removed", "Icons", "file");
    }
    return success;
}

bool FileHandler::removeDirectory(QString path)
{
    QDir dir(path);
    bool success = dir.exists() ? dir.removeRecursively() : true;

    if(!success){
        _notification(Notification::Severity::ERROR, "Directory '" % dir.absolutePath() % "' failed to be removed", "Icons", "folder");
    }
    return success;
}

QString FileHandler::getTempFileName(QString suffix, bool timestamp)
{
    return QDir::tempPath() + "/" + (timestamp ? getTimestamp() : "") + suffix;
}

void FileHandler::sanitizeFilePath(QString &filePath)
{
    filePath = filePath.replace("\\","/");
}

QString FileHandler::getTimestamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyMMdd-hhmmss");
}

QFileDialog *FileHandler::getFileDialog()
{
    return getFileHandler()->_getFileDialog();
}

QFileDialog *FileHandler::_getFileDialog()
{
    return fileDialog;
}

FileHandler *FileHandler::getFileHandler()
{
    if(!handler){
        handler = new FileHandler();
    }
    return handler;
}

void FileHandler::_notification(Notification::Severity severity, QString notificationText, QString iconPath, QString iconName, bool toast)
{
    NotificationManager::manager()->AddNotification(notificationText, iconPath, iconName, severity, Notification::Type::APPLICATION, Notification::Category::FILE, toast);
}

QString FileHandler::getAutosaveFilePath(QString path){
    if(isAutosaveFilePath(path)){
        return path;
    }else{
        QFileInfo fi(path);
        //Trim the .graphml
        auto project_path = fi.path();
        auto project_base = fi.completeBaseName();

        if(!project_base.isEmpty()){
            QString auto_path = project_path + "/" + project_base + ".autosave.graphml";
            sanitizeFilePath(auto_path);
            return auto_path;
        }else{
            return path;
        }
    }
}

bool FileHandler::isAutosaveFilePath(QString path){
    return path.endsWith(".autosave.graphml");
}

QString FileHandler::getFileFromAutosavePath(QString autosave_path){
    if(isAutosaveFilePath(autosave_path)){
        QString str = ".autosave.graphml";
        auto pos = autosave_path.lastIndexOf(str);

        if(pos >= 0){
            autosave_path.remove(pos, str.size());
        }
        return autosave_path + ".graphml";
    }else{
        return autosave_path;
    }
}