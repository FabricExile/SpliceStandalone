
#ifndef __KLSYNTAXHIGHLIGHTER_H__
#define __KLSYNTAXHIGHLIGHTER_H__

#include <QHash>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

#include <FabricCore.h>

namespace FabricSplice
{

  class KLSyntaxHighlighter : public QSyntaxHighlighter {
  public:

    KLSyntaxHighlighter(QTextDocument * document);
    bool isKeyWord(const QString & word);

  protected:
    void highlightBlock(const QString &text);

  private:
    struct HighlightingRule
    {
      QRegExp pattern;
      QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QStringList keywordPatterns;
    QStringList classPatterns;

    QTextCharFormat keywordFormat;

    QTextCharFormat classFormat;

    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;

    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;

    QTextCharFormat numberFormat;

  };
};

#endif 
