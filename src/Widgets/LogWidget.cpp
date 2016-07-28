#include <fstream>
#include <string>

#include <QVBoxLayout>

#include "LogWidget.h"

using namespace FabricSplice;

LogWidget::LogWidget(QWidget* pParent)
: QWidget(pParent)
{
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);

	m_textEdit = new QTextEdit(this);
	layout->addWidget(m_textEdit);

}

void LogWidget::clear()
{
  m_textEdit->document()->clear();
}

void LogWidget::addLog(std::string logMessage)
{
	m_textEdit->insertPlainText(logMessage.c_str());
	m_textEdit->ensureCursorVisible();
}
