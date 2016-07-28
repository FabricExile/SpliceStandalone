#ifndef __AEWIDGETFACTORY_H__
#define __AEWIDGETFACTORY_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <map>
#include <set>

#include <boost/unordered_map.hpp>
#include <QScrollArea>
#include <QVBoxLayout>

#include <FabricCore.h>
#include <FabricSplice.h>

namespace FabricSplice
{
	class AEWidget;
	class AttributeEditorWidget;

	class AEWidgetFactory;
	
	/// \brief Attribute widgets get created using this factory class
	/// 
	/// This class serve as the registry of constructors for the widgets \n
	/// use this class to add new type or get a widget based on a attribute type
	class AEWidgetFactory
	{
		public:

			/// definition of the Creation function pointer used in the factory table
			typedef AEWidget * (*AEWidgetCreateFn)(FabricSplice::DGPort port , QWidget* parent);

			//! @name Constructors / Destructors
			//@{
			AEWidgetFactory();
			~AEWidgetFactory();
			//@}

			//! @name registration
			//@{
			/// registration of the standardType
			bool registerWidgetTypes() const;
			bool registerStandardType() const;
			// bool registerPluginTypes() const;

			/// registration of creator function
			bool registerType( const std::string&  typeName, AEWidgetCreateFn creatorFn ) const;
			
			/// check is a type of registered
			bool isRegistered( const std::string&  typeName );
			//@}
			
			/// create a class, given its name. NULL if not registered
			AEWidget * create( FabricSplice::DGPort port ,
							   QWidget* parent = NULL ) const;

      static const AEWidgetFactory & widgetFactory();

		protected:

			/// the map of register creator function for the Attributes Type

			typedef boost::unordered_map< std::string , AEWidgetCreateFn, boost::hash<std::string> > TypeMap;
			mutable TypeMap m_widgetCreatorMap;
			
			/// to make sure we dont register standart type multiple times
			mutable bool	m_standardTypeRegistered;


	};
	
	
}


#endif
