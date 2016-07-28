#include "AEWidgetFactory.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QDialog>
#include <QPushButton>

#include "macros.h"

#include "AEWidget.h"
#include "BooleanAEWidget.h"
#include "Float32AEWidget.h"
#include "Float32SliderAEWidget.h"
#include "SInt32AEWidget.h"
#include "SInt32SliderAEWidget.h"
#include "StringAEWidget.h"
#include "FilePathAEWidget.h"
#include "Vec2AEWidget.h"
#include "Vec3AEWidget.h"
#include "EulerAEWidget.h"
#include "Mat44AEWidget.h"
#include "XfoAEWidget.h"
#include "ColorAEWidget.h"

using namespace FabricSplice;

AEWidgetFactory g_widgetFactory;

const AEWidgetFactory & AEWidgetFactory::widgetFactory()
{
  return g_widgetFactory;
}

AEWidgetFactory::AEWidgetFactory()
	: m_standardTypeRegistered(false)
{
}

AEWidgetFactory::~AEWidgetFactory()
{
}

bool AEWidgetFactory::registerWidgetTypes() const
{
	registerStandardType();
	return true;
	
}

bool AEWidgetFactory::registerStandardType() const
{
	if (!m_standardTypeRegistered)
	{
		registerType("Boolean", BooleanAEWidget::create );
    registerType("Float32", Float32AEWidget::create );
    registerType("Float32Slider", Float32SliderAEWidget::create );
    registerType("Scalar", Float32AEWidget::create );
    registerType("ScalarSlider", Float32SliderAEWidget::create );
    registerType("SInt32", SInt32AEWidget::create );
    registerType("SInt32Slider", SInt32SliderAEWidget::create );
    registerType("Integer", SInt32AEWidget::create );
    registerType("IntegerSlider", SInt32SliderAEWidget::create );
    registerType("Vec2", Vec2AEWidget::create );
    registerType("Vec3", Vec3AEWidget::create );
    registerType("Euler", EulerAEWidget::create );
    registerType("Mat44", Mat44AEWidget::create );
    registerType("Xfo", XfoAEWidget::create );
    registerType("Color", ColorAEWidget::create );
    registerType("String", StringAEWidget::create );
    registerType("FilePath", FilePathAEWidget::create );

		m_standardTypeRegistered = true;
		return true;
	}
	
	return false;
}

bool AEWidgetFactory::registerType(const std::string&  typeName, AEWidgetCreateFn creatorFn ) const
{
	if(m_widgetCreatorMap.find(typeName) == m_widgetCreatorMap.end())
	{
		m_widgetCreatorMap[typeName] = creatorFn;
		return true;
	}
	else	
		return false;
}

bool AEWidgetFactory::isRegistered(const std::string& typeName )
{
	return m_widgetCreatorMap.find(typeName) != m_widgetCreatorMap.end();
}

AEWidget * AEWidgetFactory::create(FabricSplice::DGPort port,
									QWidget* parent ) const
{
  FABRIC_TRY_RETURN("AEWidgetFactory::create", NULL,

    registerWidgetTypes();

    std::string dataType = port.getDataType();

    FabricCore::Variant uiMin = port.getOption("uiMin");
    FabricCore::Variant uiMax = port.getOption("uiMax");
    if(!uiMin.isNull() && !uiMax.isNull())
    {
      dataType += "Slider";      
    }

  	TypeMap::const_iterator it = m_widgetCreatorMap.find(dataType);
  	if(it == m_widgetCreatorMap.end())
  	{
  		return NULL;
  	}
  	else
  		return (it->second)(port , parent );

  );
}														

