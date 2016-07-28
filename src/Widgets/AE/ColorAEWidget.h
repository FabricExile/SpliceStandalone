#ifndef __COLORAEWIDGET_H__
#define __COLORAEWIDGET_H__

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
  class ColorPickerWidget : public QWidget
  {
    Q_OBJECT

  public:
    ColorPickerWidget(QWidget * parent);

    virtual void  mousePressEvent ( QMouseEvent * event );
    virtual void  paintEvent ( QPaintEvent * event );

    void set(float r, float g, float b, float a)
    {
      m_colorR = r;
      m_colorG = g;
      m_colorB = b;
      m_colorA = a;
      update();
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    }

    void setR(float value)
    {
      m_colorR = value; 
      update(); 
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    }

    void setG(float value)
    {
      m_colorG = value; 
      update(); 
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    }

    void setB(float value)
    {
      m_colorB = value; 
      update(); 
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    }

    void setA(float value)
    {
      m_colorA = value; 
      update(); 
      emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    }

    float getR() const { return m_colorR; }
    float getG() const { return m_colorG; }
    float getB() const { return m_colorB; }
    float getA() const { return m_colorA; }

  signals:
    void colorChanged(float r, float g, float b, float a);

  private slots:
    void colorDialogChanged(const QColor & color);
    void colorDialogRejected();

  private:
    float m_colorR;
    float m_colorG;
    float m_colorB;
    float m_colorA;
    bool m_dialogRejected;
  };

	/// \brief AEWidget to represent a Color Attribute
	class ColorAEWidget : public AEWidget
	{
		Q_OBJECT

		public:

			ColorAEWidget( FabricSplice::DGPort port ,QWidget* parent = NULL);
			~ColorAEWidget() {}

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

      typedef ColorPickerWidget ItemWidget;
		  std::vector<ItemWidget *> m_widgets;

      QWidget * m_listWidget;

	};
	
	
}


#endif
