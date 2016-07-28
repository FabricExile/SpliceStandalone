#ifndef __MAT44AEWIDGET_H__
#define __MAT44AEWIDGET_H__

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
	class Mat44AEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			Mat44AEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~Mat44AEWidget() {}

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
      std::vector<ItemWidget *> m_widgetsRow0X;
      std::vector<ItemWidget *> m_widgetsRow0Y;
      std::vector<ItemWidget *> m_widgetsRow0Z;
      std::vector<ItemWidget *> m_widgetsRow0T;
      std::vector<ItemWidget *> m_widgetsRow1X;
      std::vector<ItemWidget *> m_widgetsRow1Y;
      std::vector<ItemWidget *> m_widgetsRow1Z;
      std::vector<ItemWidget *> m_widgetsRow1T;
      std::vector<ItemWidget *> m_widgetsRow2X;
      std::vector<ItemWidget *> m_widgetsRow2Y;
      std::vector<ItemWidget *> m_widgetsRow2Z;
      std::vector<ItemWidget *> m_widgetsRow2T;
      std::vector<ItemWidget *> m_widgetsRow3X;
      std::vector<ItemWidget *> m_widgetsRow3Y;
      std::vector<ItemWidget *> m_widgetsRow3Z;
      std::vector<ItemWidget *> m_widgetsRow3T;

      QWidget * m_listWidget;
      QDoubleValidator * m_validator;

	};
	
	
}


#endif
