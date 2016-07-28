#ifndef __STRINGAEWIDGET_H__
#define __STRINGAEWIDGET_H__

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>

#include "DoubleValidator.h"
#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a String Attribute
	class StringAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			StringAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~StringAEWidget() {}

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
		  std::vector<ItemWidget *> m_widgets;

      QWidget * m_listWidget;

	};
	
	
}


#endif
