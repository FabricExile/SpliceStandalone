#ifndef __LOGWIDGET_H__
#define __LOGWIDGET_H__

#include <QWidget>
#include <QTextEdit>

namespace FabricSplice
{

  class LogWidget : public QWidget
  {
  	Q_OBJECT

  public:
  	LogWidget(QWidget* pParent = NULL);

    void clear();
  	void addLog(std::string logMessage);

  private:
  	QTextEdit * m_textEdit;

  };
};

#endif
