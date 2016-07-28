#include <QFont>
#include <QVBoxLayout>
#include <QPainter>
#include <QApplication>
#include <QScrollBar>
#include <QToolTip>

#include "KLSourceCodeWidget.h"

#include <iomanip>  
#include <string>

using namespace FabricSplice;

LineNumberWidget::LineNumberWidget(QFont font, QWidget * parent)
: QWidget(parent)
{
  mFont = font;
  mFontMetrics = new QFontMetrics(mFont);
  int maxWidth = mFontMetrics->width("0000") + 4;
  setMinimumWidth(maxWidth);
  setMaximumWidth(maxWidth);
  mLineOffset = 0;
}

LineNumberWidget::~LineNumberWidget()
{
  delete(mFontMetrics);
}

void LineNumberWidget::paintEvent ( QPaintEvent * event )
{
  QWidget::paintEvent(event);

  QPainter painter(this);
  QPalette palette(qApp->palette());
  //painter.fillRect(event->rect(), palette.color(QPalette::Background));
  painter.fillRect(event->rect(), QColor(39, 40, 34) );

  int lineHeight = mFontMetrics->height() + mFontMetrics->leading() * 2;
  int width = event->rect().width();
  int height = event->rect().height();
  int offset = lineHeight;
  if(mLineOffset == 0)
    offset += 2;
  else
    offset -= 2;

  painter.setFont(mFont);
  painter.setPen( QColor(139, 140, 135) );

  int line = mLineOffset + 1;
  char buffer[128];
  while(offset < height)
  {
    std::string paddingNumber;
#ifdef _WIN32
    itoa(line, buffer, 10);
#else
    snprintf(buffer, 128, "%d", line);
#endif
    paddingNumber = buffer;
    while(paddingNumber.length() < 4)
      paddingNumber = "0" + paddingNumber;

    int lineWidth = mFontMetrics->width(paddingNumber.c_str());
    painter.drawText(QPoint(width - 2 - lineWidth, offset), paddingNumber.c_str());
    offset += lineHeight;
    line++;
  }
}

CodeCompletionMenu::CodeCompletionMenu(QFont font, QWidget * parent)
: QMenu(parent)
{
  mFont = font;
  mFont.setPixelSize(11);
  mShown = false;
  setStyleSheet("QWidget { background-color: light-blue, foreground-color: white }");
}

void CodeCompletionMenu::addItem(const std::string & name, const std::string & desc, const std::string & comments)
{
  Item item;
  item.name = name;
  item.desc = desc;
  item.comments = comments;
  mItems.push_back(item);
}

void CodeCompletionMenu::clearItems()
{
  mItems.clear();
  mPressedKeySequence.clear();
  mPrefix.clear();
  clear();
}

void CodeCompletionMenu::show(QPoint pos)
{
  mPos = pos;
  clear();
  QAction * currentAction = NULL;
  for(size_t  i=0;i<mItems.size();i++)
  {
    mItems[i].action = NULL;
    if(mPressedKeySequence.length() > 0)
    {
      if(mItems[i].name.substr(0, mPressedKeySequence.length()) != mPressedKeySequence)
        continue;
    }
    std::string label = mItems[i].desc.c_str();
    if(label.length() > 50)
      label = label.substr(0, 47) + "...";
    mItems[i].action = addAction(label.c_str());
    if(mItems[i].comments.length() > 0)
    {
      mItems[i].action->setStatusTip(mItems[i].comments.c_str());
      mItems[i].action->setToolTip(mItems[i].comments.c_str());
    }
    else
    {
      mItems[i].action->setStatusTip(mItems[i].desc.c_str());
      mItems[i].action->setToolTip(mItems[i].desc.c_str());
    }
    mItems[i].action->setFont(mFont);
    if(currentAction == NULL)
      currentAction = mItems[i].action;
  }
  if(mItems.size() == 0)
    return;
  if(!mShown)
  {
    mShown = true;
    setActiveAction(currentAction);
    QAction * action = exec(mPos);
    if(action)
    {
      std::string text;
      for(size_t i=0;i<mItems.size();i++)
      {
        if(mItems[i].action == action)
        {
          text = mItems[i].name;
          break;
        }
      }
      text = text.substr(mPrefix.length(), 10000);
      if(text.length() > 0)
        ((PlainTextWidget*)parent())->insertPlainText(text.c_str());
    }
    ((PlainTextWidget*)parent())->setFocus();
    mShown = false;
  }
  else
  {
    setActiveAction(currentAction);
  }
}

void CodeCompletionMenu::keyPressEvent(QKeyEvent  *e)
{
  if(!mShown)
    return QMenu::keyPressEvent(e);
  if(e->key() == 16777219) // backspace
  {
    if(mPressedKeySequence.length() >  0)
    {
      mPressedKeySequence = mPressedKeySequence.substr(0, mPressedKeySequence.length()-1);
      update();
    }
    e->accept();
    return;
  }
  else if(e->key() == 16777220) // enter
  {
    return QMenu::keyPressEvent(e);
  }
  if(e->modifiers() != Qt::ShiftModifier && e->modifiers() != 0)
    return QMenu::keyPressEvent(e);

  QString text = e->text();
  if(text.length() != 1)
    return QMenu::keyPressEvent(e);
  if(isalnum(text.at(0).toAscii()))
  {
    mPressedKeySequence += text.at(0).toAscii();
    update();
    e->accept();
    // e->ignore();
    return;
  }

  QMenu::keyPressEvent(e);
}

void CodeCompletionMenu::hide()
{
  mShown = false;
}

std::string CodeCompletionMenu::completionSuffix()
{
  QAction * action = activeAction();
  if(!action)
    return "";
  QString qText = action->text();
  std::string text;
  #ifdef _WIN32
    text = qText.toLocal8Bit().constData();
  #else
    text = qText.toUtf8().constData();
  #endif
  if(text.substr(0, mPressedKeySequence.length()) == mPressedKeySequence)
    return text.substr(mPressedKeySequence.length(), 10000);
  return "";
}

PlainTextWidget::PlainTextWidget(QFont font, LineNumberWidget * lineNumbers, QWidget * parent)
: QPlainTextEdit(parent)
{
  //setRenderHint(QPainter::Antialiasing, false);

  document()->setDefaultFont(font);
  setWordWrapMode(QTextOption::NoWrap);
  setTabStopWidth(4);
  
  QPalette p = palette();
  p.setColor(QPalette::Base,QColor(39, 40, 34));
  p.setColor(QPalette::WindowText, QColor( 248, 248, 242));
  p.setColor(QPalette::Text, QColor( 248, 248, 242));
  setPalette(p);

  mLineNumbers = lineNumbers;
  mLastScrollOffset = -1;

  mHighlighter = new KLSyntaxHighlighter(this->document());

  mCodeCompletionMenu = new CodeCompletionMenu(font, this);
  mCodeCompletionMenu->clearItems();
}

PlainTextWidget::~PlainTextWidget()
{
  delete(mHighlighter);
}

bool PlainTextWidget::event(QEvent * e)
{
  if(e->type() == QEvent::ToolTip)
  {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(e);
    QTextCursor cursor = cursorForPosition(helpEvent->pos());

    KLSourceCodeWidget * sourceCodeWidget = (KLSourceCodeWidget*)parent();
    sourceCodeWidget->updateKLParser();
    KLParser parser = sourceCodeWidget->getKLParser();
    KLParser::KLSymbol symbol = parser.getKLSymbolFromCharIndex(cursor.position());
    if(!symbol)
      return QPlainTextEdit::event(e);
    if(symbol.type() != KLParser::KLSymbol::Type_name && symbol.type() != KLParser::KLSymbol::Type_rt)
      return QPlainTextEdit::event(e);

    KLParser::KLSymbol prevSymbol = symbol.prev();
    std::string toolTipText;
    if(prevSymbol.type() == KLParser::KLSymbol::Type_period)
    {
      std::string name = symbol.str();
      std::string symbolType = parser.getKLTypeForSymbol(prevSymbol.prev());
      for(unsigned int i=0;i<KLParser::getNbParsers();i++)
      {
        KLParser p = KLParser::getParser(i);
        for(unsigned int j=0;j<p.getNbKLFunctions();j++)
        {
          KLParser::KLFunction f = p.getKLFunction(j);
          if(f.owner() == symbolType && f.name() == name)
          {
            if(toolTipText.length() > 0)
              toolTipText += "\n";
            toolTipText += f.comments();
            toolTipText += "\n";
            if(strlen(f.type()) > 0)
            {
              toolTipText += f.type();
              toolTipText += " ";
            }
            if(strlen(f.owner()) > 0)
            {
              toolTipText += f.owner();
              toolTipText += ".";
            }
            toolTipText += f.name();
            toolTipText += "(";
            KLParser::KLArgumentList args = f.arguments();
            for(unsigned k=0;k<args.nbArgs();k++)
            {
              if(k>0)
                toolTipText += ", ";
              toolTipText += args.mode(k);
              toolTipText += " ";
              toolTipText += args.type(k);
              toolTipText += " ";
              toolTipText += args.name(k);
            }
            toolTipText += ")";
          }
        } 
      }
    }
    else
    {
      toolTipText = parser.getKLTypeForSymbol(symbol);
    }
    if(toolTipText.length() == 0)
      return QPlainTextEdit::event(e);
    QToolTip::showText(mapToGlobal(helpEvent->pos()), toolTipText.c_str());
    return true;
  }
  return QPlainTextEdit::event(e);
}

std::string PlainTextWidget::getStdText()
{
  QString code = toPlainText();
  return code.toUtf8().constData();
}

void PlainTextWidget::setStdText(const std::string & code)
{
  setPlainText(code.c_str());
}

void PlainTextWidget::keyPressEvent (QKeyEvent * e)
{
  if(e->key() == 16777217) // tab
  {
    insertPlainText("  ");
    return;
  }
  else if(e->key() == 16777218) // shift + tab
  {
    // for now let's just ignore this
    return;
  }
  else if(e->key() == 16777220) // enter
  {
    std::string code(getStdText());
    if(code.length() > 0)
    {
      int pos = textCursor().position();
      std::string posStr;

      // remove all previous lines
      int find = code.find('\n');
      while(find < pos && find >= 0)
      {
        code = code.substr(find + 1);
        pos -= find + 1;
        find = code.find('\n');
      }

      // figure out how many spaces to insert
      unsigned int initialPos = pos;
      pos = 0;
      std::string prefix;
      while((unsigned int) pos < code.length())
      {
        if(code.at(pos) != ' ')
          break;
        prefix += " ";
        pos++;
      }

      // remove unrequired spaces
      while(prefix.length() > 0 && code.at(initialPos) == ' ')
      {
        prefix = prefix.substr(1);
        initialPos++;
        if(initialPos == code.length())
          break;
      }

      prefix = "\n" + prefix;
      insertPlainText(prefix.c_str());
      return;
    }
  }

  if(e->key() == 32) // space
  {
    if(e->modifiers().testFlag(Qt::ControlModifier))
    {
      if(mCodeCompletionMenu->shown())
      {
        std::string suffix = mCodeCompletionMenu->completionSuffix();
        if(suffix.length() > 0)
        {
          insertPlainText(suffix.c_str());
          return;
        }
      }
      else
      {
        std::string prefix;
        KLSourceCodeWidget * sourceCodeWidget = (KLSourceCodeWidget*)parent();
        sourceCodeWidget->updateKLParser();
        KLParser parser = sourceCodeWidget->getKLParser();
        KLParser::KLSymbol symbol = parser.getKLSymbolFromCharIndex(textCursor().position());
        if(!symbol)
          return QPlainTextEdit::keyPressEvent(e);
        KLParser::KLSymbol prevSymbol = symbol.prev();
        if(symbol.type() == KLParser::KLSymbol::Type_period || prevSymbol.type() == KLParser::KLSymbol::Type_period)
        {
          if(symbol.type() == KLParser::KLSymbol::Type_period)
            symbol = prevSymbol;
          else
          {
            prefix = symbol.str();
            symbol = prevSymbol.prev();
          }
          if(!symbol)
            return QPlainTextEdit::keyPressEvent(e);

          std::string symbolType = parser.getKLTypeForSymbol(symbol);
          if(symbolType.length() == 0)
            return;

          std::map<std::string, KLParser::KLFunction> methods;
          std::map<std::string, std::string> members;

          if(symbolType.substr(symbolType.length()-1, 1) == "]")
          {
            KLParser p;
            if(symbolType.substr(symbolType.length()-2, 1) == "[")
              p = KLParser::getParser("array", "array");
            else
              p = KLParser::getParser("dict", "dict");
            for(unsigned int j=0;j<p.getNbKLFunctions();j++)
            {
              KLParser::KLFunction f = p.getKLFunction(j);
              std::string key = f.name();
              if(key[0] == '_')
                continue;
              key += "(";
              KLParser::KLArgumentList args = f.arguments();
              for(unsigned k=0;k<args.nbArgs();k++)
              {
                if(k>0)
                  key += ", ";
                key += args.mode(k);
                key += " ";
                key += args.type(k);
                key += " ";
                key += args.name(k);
              }
              key += ")";
              if(methods.find(key) != methods.end())
                continue;
              methods.insert(std::pair<std::string, KLParser::KLFunction>(key, f));
            } 
          }
          if(methods.size() == 0)
          {
            for(unsigned int i=0;i<KLParser::getNbParsers();i++)
            {
              KLParser p = KLParser::getParser(i);
              for(unsigned int j=0;j<p.getNbKLFunctions();j++)
              {
                KLParser::KLFunction f = p.getKLFunction(j);
                if(f.owner() == symbolType)
                {
                  std::string key = f.name();
                  if(key[0] == '_')
                    continue;
                  key += "(";
                  KLParser::KLArgumentList args = f.arguments();
                  for(unsigned k=0;k<args.nbArgs();k++)
                  {
                    if(k>0)
                      key += ", ";
                    key += args.mode(k);
                    key += " ";
                    key += args.type(k);
                    key += " ";
                    key += args.name(k);
                  }
                  key += ")";
                  if(methods.find(key) != methods.end())
                    continue;
                  methods.insert(std::pair<std::string, KLParser::KLFunction>(key, f));
                }
              } 
              for(unsigned int j=0;j<p.getNbKLStructs();j++)
              {
                KLParser::KLStruct f = p.getKLStruct(j);
                if(f.name() == symbolType)
                {
                  for(unsigned k=0;k<f.nbMembers();k++)
                  {
                    std::string key = f.memberName(k);
                    if(key[0] == '_')
                      continue;
                    key += " (";
                    key += f.memberType(k);
                    key += ")";

                    if(members.find(key) != members.end())
                      continue;
                    members.insert(std::pair<std::string, std::string>(key, f.memberName(k)));
                  }
                }
              } 
            }
          }

          mCodeCompletionMenu->clearItems();
          if(methods.size() == 0 && members.size() == 0)
            return;

          mCodeCompletionMenu->setPrefix(prefix);
          for(std::map<std::string, std::string>::iterator it = members.begin();it!=members.end();it++)
          {
            mCodeCompletionMenu->addItem(it->second, it->first, "");
          }
          for(std::map<std::string, KLParser::KLFunction>::iterator it = methods.begin();it!=methods.end();it++)
          {
            mCodeCompletionMenu->addItem(it->second.name(), it->first, it->second.comments());
          }

          QPoint pos = mapToGlobal(cursorRect().topLeft() + QPoint(0, font().pixelSize()));
          mCodeCompletionMenu->show(pos);
          return;
        }

        if(symbol.type() != KLParser::KLSymbol::Type_name && symbol.type() != KLParser::KLSymbol::Type_rt)
          return QPlainTextEdit::keyPressEvent(e);
        std::string name = symbol.str();

        std::map<std::string, std::string> matches;
        for(unsigned int i=0;i<KLParser::getNbParsers();i++)
        {
          KLParser p = KLParser::getParser(i);
          for(unsigned int j=0;j<p.getNbKLStructs();j++)
          {
            KLParser::KLStruct f = p.getKLStruct(j);
            std::string key = f.name();
            if(strlen(f.type()) > 0)
            {
              key += " (";
              key += f.type();
              key += ")";
            }
            if(matches.find(key) != matches.end())
              continue;
            matches.insert(std::pair<std::string, std::string>(key, f.name()));
          } 
        }
        for(unsigned int j=0;j<parser.getNbKLVariables();j++)
        {
          KLParser::KLVariable f = parser.getKLVariable(j);
          if(f.symbol().pos() >= symbol.pos())
            continue;
          std::string match(f.name());
          if(match.substr(0,name.length()) == name)
          {
            std::string key = f.name();
            key += " (";
            key += f.type();
            key += ")";
            if(matches.find(key) != matches.end())
              continue;
            matches.insert(std::pair<std::string, std::string>(key, f.name()));
          }
        }

        for(unsigned int i=0;i<KLParser::getNbParsers();i++)
        {
          KLParser p = KLParser::getParser(i);
          for(unsigned int j=0;j<p.getNbKLConstants();j++)
          {
            KLParser::KLConstant f = p.getKLConstant(j);
            std::string match(f.name());
            if(match.substr(0,name.length()) == name)
            {
              std::string key = f.name();
              if(key[0] == '_')
                continue;
              key += " = ";
              key += f.value();
              if(matches.find(key) != matches.end())
                continue;
              matches.insert(std::pair<std::string, std::string>(key, f.name()));
            }
          }
          for(unsigned int j=0;j<p.getNbKLStructs();j++)
          {
            KLParser::KLStruct f = p.getKLStruct(j);
            std::string match(f.name());
            if(match.substr(0,name.length()) == name)
            {
              std::string key = f.name();
              if(key[0] == '_')
                continue;
              key += " (";
              key += f.type();
              key += ")";
              if(matches.find(key) != matches.end())
                continue;
              matches.insert(std::pair<std::string, std::string>(key, f.name()));
            }
          }           
          for(unsigned int j=0;j<p.getNbKLFunctions();j++)
          {
            KLParser::KLFunction f = p.getKLFunction(j);
            std::string match(f.name());
            if(strlen(f.owner()) == 0 && match.substr(0,name.length()) == name)
            {
              std::string key = f.name();
              if(key[0] == '_')
                continue;
              key += "(";
              KLParser::KLArgumentList args = f.arguments();
              for(unsigned k=0;k<args.nbArgs();k++)
              {
                if(k>0)
                  key += ", ";
                key += args.mode(k);
                key += " ";
                key += args.type(k);
                key += " ";
                key += args.name(k);
              }
              key += ")";
              if(matches.find(key) != matches.end())
                continue;
              matches.insert(std::pair<std::string, std::string>(key, f.name()));
            }
          } 
        }

        mCodeCompletionMenu->clearItems();
        mCodeCompletionMenu->setPrefix(name);

        if(matches.size() == 0)
          return QPlainTextEdit::keyPressEvent(e);
        else if(matches.size() == 0)
        {
          insertPlainText(matches.begin()->first.c_str());
          return;
        }
        
        std::map<std::string, std::string>::iterator it;
        for(it = matches.begin();it!=matches.end();it++)
          mCodeCompletionMenu->addItem(it->second, it->first, "");

        QPoint pos = mapToGlobal(cursorRect().topLeft() + QPoint(0, font().pixelSize()));
        mCodeCompletionMenu->setPressedKeySequence(name);
        mCodeCompletionMenu->show(pos);

        return;

      }
    }
  }

  QPlainTextEdit::keyPressEvent(e);
}

void PlainTextWidget::paintEvent ( QPaintEvent * event )
{
  QPlainTextEdit::paintEvent(event);
  if(mLastScrollOffset == verticalScrollBar()->value())
    return;
  mLastScrollOffset = verticalScrollBar()->value();
  mLineNumbers->setLineOffset(mLastScrollOffset);
}

KLSourceCodeWidget::KLSourceCodeWidget(QWidget * parent, int fontSize)
: QWidget(parent)
{
  QFont font("DejaVu Sans Mono", fontSize);
  setLayout(new QHBoxLayout());
  layout()->setContentsMargins(0, 0, 0, 0);
  mLineNumbers = new LineNumberWidget(font, this);
  layout()->addWidget(mLineNumbers);
  mTextEdit = new PlainTextWidget(font, mLineNumbers, this);
  layout()->addWidget(mTextEdit);
}

std::string KLSourceCodeWidget::getSourceCode()
{
  return mTextEdit->getStdText();
}

void KLSourceCodeWidget::setSourceCode(const std::string & opName, const std::string & code)
{
  mOperator = opName;
  mTextEdit->setStdText(code);
  updateKLParser();
}

void KLSourceCodeWidget::setEnabled(bool enabled)
{
  mTextEdit->setEnabled(enabled);
}

void KLSourceCodeWidget::updateKLParser()
{
  mParser = KLParser::getParser(mOperator.c_str(), mOperator.c_str(), getSourceCode().c_str());
}
