#ifndef CODEBROWSER_H
#define CODEBROWSER_H

#include "codeeditor.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>
class CodeBrowser : public QWidget
{
    Q_OBJECT
public:
    CodeBrowser(QWidget *parent = 0);
public slots:
    void showCode(QString fileName, QString content, bool editable);
private slots:
    void closeTab(int tabID);

private:
    void setupLayout();
    CodeEditor* getCodeEditor(QString fileName);
    QString getCodeEditorFileName(CodeEditor* editor);

    QVBoxLayout* layout;
    QTabWidget* tabWidget;
    QHash<QString, CodeEditor*> codeEditorLookup;
};

#endif // CODEBROWSER_H
