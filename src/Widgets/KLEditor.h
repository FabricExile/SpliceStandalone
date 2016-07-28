#ifndef __KLEDITOR_H__
#define __KLEDITOR_H__

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QTabWidget>

#include "SpliceGraphWrapper.h"
#include "KLSourceCodeWidget.h"

namespace FabricSplice
{

  class KLEditor : public QWidget
  {
  	Q_OBJECT

  public:
  	KLEditor(QWidget* pParent = NULL);
  	
    void clear();
  	void setWrapper(SpliceGraphWrapper::Ptr wrapper);

  public slots:
    void compilePressed();

  private:

    void saveEditorCodeToDisk(std::string path);

  	KLSourceCodeWidget * m_sourceCodeWidget;
  	SpliceGraphWrapper::Ptr m_editorWrapper;	

  };
};

#endif
