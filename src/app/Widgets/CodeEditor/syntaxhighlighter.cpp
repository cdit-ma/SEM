#include <QtGui>

#include "syntaxhighlighter.h"
#include "../../theme.h"

TextBlockData::TextBlockData()
{
    // Nothing to do
}

QVector<ParenthesisInfo *> TextBlockData::parentheses()
{
    return m_parentheses;
}

void TextBlockData::insert(ParenthesisInfo *info)
{
    int i = 0;
    while (i < m_parentheses.size() &&
        info->position > m_parentheses.at(i)->position)
        ++i;

    m_parentheses.insert(i, info);
}


//asdasd
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    connect(Theme::theme(), &Theme::theme_Changed, this, &SyntaxHighlighter::themeChanged);

    themeChanged();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

    highlightParenthesis(text);
}

void SyntaxHighlighter::highlightParenthesis(const QString &text) {
    TextBlockData *data = new TextBlockData;


    QRegExp rx("\\(|\\)|\\{|\\}|\\[|\\]|\\<|\\>");
    int pos = 0;
    while(true){
        pos = text.indexOf(rx, pos);
        if(pos != -1){
            ParenthesisInfo* info = new ParenthesisInfo();
            info->character = text.at(pos).toLatin1();
            //qCritical() << info->character;
            //qCritical() << pos;
            info->position = pos;

            data->insert(info);
            pos ++;
        }else{
            break;
        }
    }
    setCurrentBlockUserData(data);
}

void SyntaxHighlighter::themeChanged()
{
    highlightingRules.clear();

    quotationFormat.setForeground(Theme::theme()->getTextColor(ColorRole::DISABLED));
    singleLineCommentFormat.setForeground(Theme::theme()->getTextColor(ColorRole::DISABLED));
    multiLineCommentFormat.setForeground(Theme::theme()->getTextColor(ColorRole::DISABLED));


    //singleLineCommentFormat.setForeground(Theme::theme()->getHighlightColor());
    //multiLineCommentFormat.setForeground(Theme::theme()->getHighlightColor());
    functionFormat.setForeground(Theme::theme()->getHighlightColor());
    keywordFormat.setForeground(Theme::theme()->getHighlightColor());

    keywordFormat.setFontWeight(QFont::Bold);
    HighlightingRule rule;
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bfloat\\b" << "\\bextern\\b"
                    << "\\b#include\\b" << "\\bvirtual\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    {
        auto str_reg = QRegExp("\".*\"");
        //Don't be greedy
        str_reg.setMinimal(true);
        rule.pattern = str_reg;
    }
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);

    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

bool ParenthesisInfo::isOpening() const
{
    return character == '(' || character == '{' || character == '[' || character == '<';

}

char ParenthesisInfo::getMatching() const
{
    switch(character){
    case '(':
        return ')';
    case ')':
        return '(';
    case '[':
        return ']';
    case ']':
        return '[';
    case '{':
        return '}';
    case '}':
        return '{';
    case '<':
        return '>';
    case '>':
        return '<';
    default:
        return character;
    }
}




