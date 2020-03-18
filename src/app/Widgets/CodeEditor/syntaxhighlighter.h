#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>
#include <QRegularExpression>

class QTextDocument;

struct ParenthesisInfo
{
	char character;
	int position;
	bool isOpening() const;
	char getMatching() const;
};

class TextBlockData : public QTextBlockUserData
{
public:
	TextBlockData() = default;
	
	QVector<ParenthesisInfo *> parentheses();
	void insert(ParenthesisInfo *info);

private:
	QVector<ParenthesisInfo *> m_parentheses;
};


class SyntaxHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
	explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
	void highlightBlock(const QString &text) override;
	void highlightParenthesis(const QString &text);

private:
	void themeChanged();
	
	struct HighlightingRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	
	QVector<HighlightingRule> highlightingRules;
	
	QTextCharFormat keywordFormat;
	QTextCharFormat commentFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat functionFormat;
	QTextCharFormat templateFormat;
};

#endif // SYNTAXHIGHLIGHTER_H