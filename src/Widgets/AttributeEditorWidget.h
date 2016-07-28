#ifndef __ATTRIBUTEEDITOREWIDGET_H__
#define __ATTRIBUTEEDITOREWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>

#include <map>
#include <set>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>

#include <boost/shared_ptr.hpp>
#include <FabricCore.h>

#include "SpliceGraphWrapper.h"

namespace FabricSplice
{
#ifndef __NODEDATA_FWD__
#define __NODEDATA_FWD__

  class NodeData;
  typedef boost::shared_ptr<NodeData> NodeDataPtr;

#endif


	class AEWidget;	
	class AEWidgetFactory;
	
	/// \brief Top level Widget for the Attribute Editor.
	/// 
	/// This class act as booth the Logic controller and the Display widget.\n
	/// it contain all the Attributes displaying the values of the Table
	class AttributeEditorWidget : public QWidget
	{
		Q_OBJECT

		public:

			/// FilterMode
			typedef enum {
				AEFilterModeRegEx = 0, /*!< use Regular Expression to filter the attributes. */ 
				AEFilterModeWildcard,  /*!< use simple Wildcard to filter the attributes. */ 
				AEFilterModeString,	   /*!< simply serach for the string equivalent to searching for "*myString*" */
			} AEFilterMode;
			
			typedef boost::shared_ptr<AttributeEditorWidget> Ptr;

			/// contructor that create the child UI Elements
			AttributeEditorWidget(QWidget* parent = NULL);
			~AttributeEditorWidget();

      /// clears all widgets
      void clearAllWidgets();

      /// set the internal data from a wrapper
      void setWrapper(SpliceGraphWrapper::Ptr wrapper);

      /// update the args and also make sure the widget is reflecting this new value 
      void setWidgetPort( std::string name, FabricSplice::DGPort port, bool silent = true );
      
      /// initial brute for setting up the widget to their current value
      void updateAllWidgets();
  
      /// return a the widget for that name 
      AEWidget * getWidget(std::string argName);
      
		public slots:
			
			/// this need to connected to the uiChanged methods of AEWidget
			/// added to the tree 
			virtual void widgetValueChanged(std::string attributeName);
	
		private slots:
			/// mostly for debuging , activate the printArgs button when the args are dirty
			/// void activatePrintButton();
					
		signals:
			/// notify that one of the child  data could be wrong
			void valueForWidgetChanged( std::string attributeName );
	
			/// this is emited anytime of the the args is invalidate
			void attributeChanged( QSpliceGraphWrapper wrapper, std::string attributeName );
			
		protected:

			//! @name UI Elements
			//@{
			QScrollArea	* m_mainScollArea;
			QVBoxLayout * m_scrollLayout;
			QFrame		* m_mainFrame;

			/// this is a potential debug button ( normally not added or created )
			QPushButton * m_printArgsButton;

			/// a map of widget for names
			typedef std::map< std::string , AEWidget * > WidgetMap;
			WidgetMap m_widgetMap;

      SpliceGraphWrapper::Ptr m_wrapper;
	};
	
}


#endif
