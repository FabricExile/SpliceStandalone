#include "FilePathAEWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QSizePolicy>
#include <QtGui/QFileDialog>

#include "macros.h"
#include "utility.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * FilePathAEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new FilePathAEWidget(port ,parent);
}

FilePathAEWidget::FilePathAEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal FilePathAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("FilePathAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("FilePath");
    rtValSetArraySize(values, m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      std::string path = stdStringFromQString(m_widgets[i]->text());
      FabricCore::RTVal pathVal = constructStringRTVal(path.c_str());
      rtValSetArrayElement(values, i, constructObjectRTVal("FilePath", 1, &pathVal));
    }
  );
	return values;
}

void FilePathAEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("FilePathAEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void FilePathAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("FilePathAEWidget::setValueArray", 

    // clear the layout
    if(rtValGetArraySize(values) != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      QGridLayout * layout = (QGridLayout *)m_listWidget->layout();

      m_widgets.resize(rtValGetArraySize(values));
      m_buttons.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );

        m_buttons[i] = new QPushButton("...", m_listWidget);
        m_buttons[i]->setFont( getApplicationWidgetFont() );
        m_buttons[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        connect( m_buttons[i], SIGNAL( clicked() ), this , SLOT( browseClicked() ) );
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        layout->addWidget( m_widgets[i], i, 0);
        layout->addWidget( m_buttons[i], i, 1);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      FabricCore::RTVal pathObj = rtValGetArrayElement(values, i);
      FabricCore::RTVal pathStr = pathObj.callMethod("String", "string", 0, 0);
      m_widgets[i]->setText(pathStr.getStringCString());
    }

  );
}

void FilePathAEWidget::uiChanged()
{
  FABRIC_TRY("FilePathAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}

void FilePathAEWidget::browseClicked()
{
  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_buttons.size();i++)
  {
    if(m_buttons[i] != sender)
      continue;

    boost::filesystem::path path = stdStringFromQString(m_widgets[i]->text());
    std::string directory;
    std::string filter;
#if BOOST_VERSION == 105500
    if(!path.has_extension())
#else
    if(path.extension().empty())
#endif
    {
      directory = path.string();
      filter = "All files (*.*)";
    }
    else
    {
      directory = path.parent_path().string();
#if BOOST_VERSION == 105500
      std::string ext = path.extension().string().substr(1, 1000);
#else
      std::string ext = path.extension().substr(1, 1000);
#endif
      filter = ext + " files (*." + ext + ");;All files (*.*)";
    }

    std::string name;
    std::string mode;
    FABRIC_TRY("FilePathAEWidget::browseClicked", 
      name = m_port.getName();
      mode = m_port.getMode() == FabricSplice::Port_Mode_IN ? "in" : "io";
    );

    QString filePath;

    if(mode == "in")
      filePath = QFileDialog::getOpenFileName(this, name.c_str(), directory.c_str(), filter.c_str());
    else
      filePath = QFileDialog::getSaveFileName(this, name.c_str(), directory.c_str(), filter.c_str());
    if(filePath.isEmpty())
      return;

    m_widgets[i]->setText(filePath);
    break;
  }
}
