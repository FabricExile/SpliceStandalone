#ifndef __VEC3AEWIDGET_H__
#define __VEC3AEWIDGET_H__

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
	class Vec3AEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			Vec3AEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~Vec3AEWidget() {}

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
      std::vector<ItemWidget *> m_widgetsX;
      std::vector<ItemWidget *> m_widgetsY;
		  std::vector<ItemWidget *> m_widgetsZ;

      QWidget * m_listWidget;
      QDoubleValidator * m_validator;

	};
	
	
}


#endif
