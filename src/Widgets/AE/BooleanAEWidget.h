#ifndef __BOOLEANAEWIDGET_H__
#define __BOOLEANAEWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QCheckBox>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a Boolean Attribute
	class BooleanAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			BooleanAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~BooleanAEWidget() {}

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

      typedef QCheckBox ItemWidget;
		  std::vector<ItemWidget *> m_widgets;

      QWidget * m_listWidget;

	};
	
	
}


#endif
