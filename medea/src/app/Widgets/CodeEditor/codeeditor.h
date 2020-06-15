#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QSyntaxHighlighter>
#include <QPlainTextEdit>

#include "syntaxhighlighter.h"

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget* parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void gotoLine();
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &, int);

    void matchParentheses();
    void themeChanged();

private:
	bool matchOtherParenthesis(ParenthesisInfo* p, QTextBlock currentBlock, int index, int openCount = 0);
	void createParenthesisSelection(int pos);
	
	SyntaxHighlighter* highlighter = nullptr;
	LineNumberArea* lineNumberArea = nullptr;
	
	QColor selectedColor;
	QColor textColor;
	QColor selectedTextColor;
	QColor altBackgroundColor;

    QList<QTextEdit::ExtraSelection> highlights;
};


class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
        setContentsMargins(0,0,0,0);
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return {codeEditor->lineNumberAreaWidth(), codeEditor->height()};
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor* codeEditor = nullptr;
};

#endif // CODEEDITOR_H