#include "ColorAEWidget.h"

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
#include <QtGui/QColorDialog>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

ColorPickerWidget::ColorPickerWidget(QWidget * parent)
  : QWidget(parent)
{
  setMinimumSize(10, 25);
  setMaximumSize(100, 25);
  m_colorR = 1.0;
  m_colorG = 0.0;
  m_colorB = 0.0;
  m_colorA = 1.0;
}

void ColorPickerWidget::mousePressEvent ( QMouseEvent * event )
{
  float prevR = m_colorR;
  float prevG = m_colorG;
  float prevB = m_colorB;
  float prevA = m_colorA;
  m_dialogRejected = false;

  QColorDialog dialog( this );
  dialog.setOptions(QColorDialog::ShowAlphaChannel);
  dialog.setModal(true);
  dialog.setCurrentColor(QColor(m_colorR * 255.0f, m_colorG * 255.0f, m_colorB * 255.0f, m_colorA * 255.0f));
  connect(&dialog, SIGNAL(rejected()), this, SLOT(colorDialogRejected()));
  connect(&dialog, SIGNAL(currentColorChanged(const QColor &)), this, SLOT(colorDialogChanged(const QColor &)));

  dialog.exec();

  if(m_dialogRejected)
  {
    m_colorR = prevR;
    m_colorG = prevG;
    m_colorB = prevB;
    m_colorA = prevA;
    emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
    update();
  }
}

void ColorPickerWidget::paintEvent ( QPaintEvent * event )
{
  QPainter painter;
  painter.begin(this);
  painter.fillRect(event->rect(), QBrush(QColor(m_colorR * 255.0f, m_colorG * 255.0f, m_colorB * 255.0f)));
  painter.end();  
}

void ColorPickerWidget::colorDialogChanged(const QColor & color)
{
  m_colorR = color.redF();
  m_colorG = color.greenF();
  m_colorB = color.blueF();
  m_colorA = color.alphaF();
  emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
  update();
}

void ColorPickerWidget::colorDialogRejected()
{
  m_dialogRejected = true;
}

AEWidget * ColorAEWidget::create( FabricCore::RTVal param , QWidget* parent)
{
	return new ColorAEWidget(param ,parent);
}

ColorAEWidget::ColorAEWidget(FabricCore::RTVal param ,QWidget* parent)
    : AEWidget(param,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  setRTVal(param);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal ColorAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("ColorAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = constructRTVal("Color[]");
    values.setArraySize(m_widgets.size());

    std::vector<FabricCore::RTVal> args(4);
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      args[0] = constructFloat32RTVal(m_widgets[i]->getR());
      args[1] = constructFloat32RTVal(m_widgets[i]->getG());
      args[2] = constructFloat32RTVal(m_widgets[i]->getB());
      args[3] = constructFloat32RTVal(m_widgets[i]->getA());
      FabricCore::RTVal color = constructRTVal("Color", 4, &args[0]);
      values.setArrayElement(i, color);
    }

  );
	return values;
}

void ColorAEWidget::setRTVal(FabricCore::RTVal param)
{
  FABRIC_TRY("ColorAEWidget::setRTVal",
  
    AEWidget::setRTVal(constructObjectRTVal("ColorParameter", 1, &param));
    setValueArray(m_param.callMethod("Color[]", "getValueArray", 0, 0));

  );
}

void ColorAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("ColorAEWidget::setValueArray", 

    // clear the layout
    if(values.getArraySize() != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_widgets.resize(values.getArraySize());
      for (unsigned int i = 0; i < values.getArraySize(); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( colorChanged(float, float, float, float) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < values.getArraySize(); ++i)
    {
      FabricCore::RTVal color = values.getArrayElement(i);
      m_widgets[i]->set(
        color.maybeGetMember("r").getFloat32(),
        color.maybeGetMember("g").getFloat32(),
        color.maybeGetMember("b").getFloat32(),
        color.maybeGetMember("a").getFloat32()
      );
    }

  );
}

void ColorAEWidget::uiChanged()
{
  FABRIC_TRY("ColorAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_param.callMethod("", "setValueArray", 1, &values);

  );
  AEWidget::uiChanged();
}
