#ifndef UTILS_H
#define UTILS_H

#include <QString>
class Utils{
    public:

    static QString readTextFile(QString file_path);
    static bool writeTextFile(QString file_path, QString file_data);
};
#endif // UTILS_H
