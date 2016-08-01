#include <QtWidgets>
#include <QFont>

#include "codeeditor.h"
#include "syntaxhighlighter.h"
#include "../View/theme.h"
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    highlighter = new SyntaxHighlighter(this->document());


    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchParentheses()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}



int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 5 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

// walk through and check that we don't exceed 80 chars per line
void CodeEditor::matchParentheses()
{
    highlightCurrentLine();


    TextBlockData *data = static_cast<TextBlockData *>(textCursor().block().userData());

    if (data) {
        QVector<ParenthesisInfo *> infos = data->parentheses();

        int pos = textCursor().block().position();
        for (int i = 0; i < infos.size(); ++i) {
            ParenthesisInfo *info = infos.at(i);

            int curPos = textCursor().position() - textCursor().block().position();
            if (info->position == curPos - 1 && info->character == '(') {
                if (matchLeftParenthesis(textCursor().block(), i + 1, 0)) {
                    createParenthesisSelection(pos + info->position);
                }
            } else if (info->position == curPos - 1 && info->character == ')') {
                if (matchRightParenthesis(textCursor().block(), i - 1, 0)) {
                    createParenthesisSelection(pos + info->position);
                }
            }
        }
    }
}

void CodeEditor::themeChanged()
{
    Theme* t = Theme::theme();

    selectedColor = t->getHighlightColor();
    selectedTextColor = t->getTextColor(Theme::CR_SELECTED);
    textColor = t->getTextColor();
    altBackgroundColor = t->getAltBackgroundColor();

    setStyleSheet("QPlainTextEdit{background:" + t->getBackgroundColorHex()+";color:"+ t->getTextColorHex()+";}");

}

bool CodeEditor::matchLeftParenthesis(QTextBlock currentBlock, int i, int numLeftParentheses)
{
    TextBlockData *data = static_cast<TextBlockData *>(currentBlock.userData());
    QVector<ParenthesisInfo *> infos = data->parentheses();

    int docPos = currentBlock.position();
    for (; i < infos.size(); ++i) {
        ParenthesisInfo *info = infos.at(i);

        if (info->character == '(') {
            ++numLeftParentheses;
            continue;
        }

        if (info->character == ')' && numLeftParentheses == 0) {
            createParenthesisSelection(docPos + info->position);
            return true;
        } else
            --numLeftParentheses;
    }

    currentBlock = currentBlock.next();
    if (currentBlock.isValid())
        return matchLeftParenthesis(currentBlock, 0, numLeftParentheses);

    return false;
}

bool CodeEditor::matchRightParenthesis(QTextBlock currentBlock, int i, int numRightParentheses)
{
    TextBlockData *data = static_cast<TextBlockData *>(currentBlock.userData());
    QVector<ParenthesisInfo *> parentheses = data->parentheses();

    int docPos = currentBlock.position();
    for (; i > -1 && parentheses.size() > 0; --i) {
        ParenthesisInfo *info = parentheses.at(i);
        if (info->character == ')') {
            ++numRightParentheses;
            continue;
        }
        if (info->character == '(' && numRightParentheses == 0) {
            createParenthesisSelection(docPos + info->position);
            return true;
        } else
            --numRightParentheses;
    }

    currentBlock = currentBlock.previous();
    if (currentBlock.isValid())
        return matchRightParenthesis(currentBlock, 0, numRightParentheses);

    return false;
}

void CodeEditor::highlightCurrentLine()
{
    highlights.clear();

    QTextEdit::ExtraSelection selection;

    QColor lineColor = Theme::theme()->getHighlightColor();

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    highlights.append(selection);

    setExtraSelections(highlights);
}

void CodeEditor::createParenthesisSelection(int pos)
{

    QTextEdit::ExtraSelection selection;
    QTextCharFormat format = selection.format;
    format.setBackground(selectedColor);
    format.setForeground(selectedTextColor);
    QFont boldFont;
    boldFont.setBold(true);
    format.setFont(boldFont);
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = cursor;

    highlights.append(selection);

    setExtraSelections(highlights);
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);

    painter.setPen(Qt::NoPen);
    painter.setBrush(altBackgroundColor);
    painter.drawRect(event->rect());

    QFont font = this->font();
    QTextBlock block = firstVisibleBlock();

    while (block.isValid()){
        QString number = QString::number(block.blockNumber() + 1);

        QRectF br = blockBoundingGeometry(block);
        br.setWidth(lineNumberAreaWidth());
        br.translate(contentOffset());

        if(block.blockNumber() == textCursor().blockNumber()){
            font.setBold(true);
            painter.setBrush(selectedColor);
            painter.setPen(Qt::NoPen);
            painter.drawRect(br);
            painter.setPen(selectedTextColor);
        } else {
            font.setBold(false);
            painter.setPen(textColor);
        }


        painter.setFont(font);
        painter.drawText(br, Qt::AlignCenter, number);
        block = block.next();
    }
}
