#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>
#include <QObject>

#include "syntaxhighlighter.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;


class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void gotoLine();
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);

    void matchParentheses();
    void themeChanged();

private:
    SyntaxHighlighter *highlighter;
    QWidget *lineNumberArea;
    QColor selectedColor;
    QColor textColor;
    QColor selectedTextColor;
    QColor altBackgroundColor;

    bool matchOtherParenthesis(ParenthesisInfo* p, QTextBlock currentBlock, int index, int openCount=0);


    void createParenthesisSelection(int pos);

    QList<QTextEdit::ExtraSelection> highlights;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
        this->setContentsMargins(0,0,0,0);
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(codeEditor->lineNumberAreaWidth(), codeEditor->height());
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};


#endif
