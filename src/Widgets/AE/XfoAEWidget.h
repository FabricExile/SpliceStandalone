#ifndef __XFOAEWIDGET_H__
#define __XFOAEWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QScrollArea>
#include <QGridLayout>
#include <QLineEdit>
#include <QDoubleValidator>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a Float32 Attribute
	class XfoAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			XfoAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~XfoAEWidget() {}

			// creation function
			static AEWidget * create( FabricSplice::DGPort port ,QWidget* parent = NULL);

			// convenient function to get the value of the check as a bool
			FabricCore::RTVal getValueArray();

      // sets the wrapped DGPort
      virtual void setPort(FabricSplice::DGPort port);

		public slots:

			void setValueArray(FabricCore::RTVal values);

    protected slots:
      virtual void uiChanged();

		private slots:

		signals:

		protected:


		protected:


		protected:

      typedef QLineEdit ItemWidget;
      std::vector<ItemWidget *> m_widgetsTrX;
      std::vector<ItemWidget *> m_widgetsTrY;
      std::vector<ItemWidget *> m_widgetsTrZ;
      std::vector<ItemWidget *> m_widgetsRotX;
      std::vector<ItemWidget *> m_widgetsRotY;
      std::vector<ItemWidget *> m_widgetsRotZ;
      std::vector<ItemWidget *> m_widgetsSclX;
      std::vector<ItemWidget *> m_widgetsSclY;
      std::vector<ItemWidget *> m_widgetsSclZ;

      QWidget * m_listWidget;
      QDoubleValidator * m_validator;

	};
	
	
}


#endif
