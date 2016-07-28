
#ifndef __KLSOURCECODEWIDGET_H__
#define __KLSOURCECODEWIDGET_H__

#include <QWidget>
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QFont>
#include <QFontMetrics>
#include <QMenu>

#include "KLSyntaxHighlighter.h"
#include <FabricSplice.h>

namespace FabricSplice {

  class LineNumberWidget : public QWidget {
  public:
    LineNumberWidget(QFont font, QWidget * parent);
    ~LineNumberWidget();
    void paintEvent ( QPaintEvent * event );
    void setLineOffset(int lineOffset) { mLineOffset = lineOffset; update(); }
  private:
    QFont mFont;
    QFontMetrics * mFontMetrics;
    int mLineOffset;
  };

  class CodeCompletionMenu : public QMenu {
  public:
    CodeCompletionMenu(QFont font, QWidget * parent = NULL);
    ~CodeCompletionMenu() {}

    void addItem(const std::string & name, const std::string & desc, const std::string & comments);
    void setPrefix(const std::string & seq) { mPrefix = seq; setPressedKeySequence(seq); }
    void setPressedKeySequence(const std::string & seq) { mPressedKeySequence = seq; }
    void clearItems();
    void show(QPoint pos);
    void update() { if(mShown)show(mPos); }
    void hide();
    bool shown() { return mShown; }
    void keyPressEvent(QKeyEvent  *e);
    std::string completionSuffix();
  private:
    struct Item{
      QAction * action;
      std::string name;
      std::string desc;
      std::string comments;
    };
    QFont mFont;
    bool mShown;
    QPoint mPos;
    std::vector<Item> mItems;
    std::string mPrefix;
    std::string mPressedKeySequence;
  };

  class PlainTextWidget : public QPlainTextEdit {
  public:
    PlainTextWidget(QFont font, LineNumberWidget * lineNumbers, QWidget * parent);
    ~PlainTextWidget();
    bool event(QEvent * e);
    void keyPressEvent(QKeyEvent  *e);
    void paintEvent ( QPaintEvent * event );

    std::string getStdText();
    void setStdText(const std::string & code);
  private:
    LineNumberWidget * mLineNumbers;
    KLSyntaxHighlighter * mHighlighter;
    CodeCompletionMenu * mCodeCompletionMenu;
    int mLastScrollOffset;
  };

  class KLSourceCodeWidget : public QWidget {
    friend class PlainTextWidget;
  public:
    KLSourceCodeWidget(QWidget * parent, int fontSize = 10);

    std::string getSourceCode();
    void setSourceCode(const std::string & opName, const std::string & code);
    void setEnabled(bool enabled);
    FabricSplice::KLParser getKLParser() { return mParser; }
    void updateKLParser();

  private:
    LineNumberWidget * mLineNumbers;
    PlainTextWidget * mTextEdit;
    std::string mOperator;
    FabricSplice::KLParser mParser;
  };
};

#endif 
