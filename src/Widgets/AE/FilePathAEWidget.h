#ifndef __FILEPATHAEWIDGET_H__
#define __FILEPATHAEWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QScrollArea>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a String Attribute
	class FilePathAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			FilePathAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~FilePathAEWidget() {}

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
      virtual void browseClicked();

		private slots:

		signals:

		protected:


		protected:


		protected:

      typedef QLineEdit ItemWidget;
      std::vector<ItemWidget *> m_widgets;
		  std::vector<QPushButton *> m_buttons;

      QWidget * m_listWidget;

	};
	
	
}


#endif
