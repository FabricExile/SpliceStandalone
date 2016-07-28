#ifndef __SINT32SLIDERAEWIDGET_H__
#define __SINT32SLIDERAEWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <map>
#include <set>
#include <vector>
#include <QScrollArea>
#include <QGridLayout>
#include <QSlider>
#include <QLineEdit>
#include <QIntValidator>

#include "AEWidget.h"

#include "macros.h"
namespace FabricSplice
{
	/// \brief AEWidget to represent a SInt32 Attribute
	class SInt32SliderAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			SInt32SliderAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~SInt32SliderAEWidget() {}

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
      virtual void sliderMoved(int pos);
      virtual void textChanged(const QString & text);

		private slots:

		signals:

		protected:


		protected:


		protected:

      typedef QLineEdit ItemWidget;
      std::vector<QSlider *> m_sliders;
		  std::vector<ItemWidget *> m_widgets;
      bool m_notifying;

      QWidget * m_listWidget;
      QIntValidator * m_validator;

	};
	
	
}


#endif
