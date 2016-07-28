#ifndef __AEWIDGET_H__
#define __AEWIDGET_H__

#include <QWidget>
#include <QContextMenuEvent>
#include <QLabel>

#include <QScrollArea>
#include <QVBoxLayout>
#include <iostream>

#include <FabricCore.h>

#include "macros.h"

namespace FabricSplice
{
	class AttributeEditorWidget;

	/// \brief Base class for all the Attribute Editor Widget
	/// 
	/// any Widget that want to be part of an AE need to have this class as on of its parent \n
	/// note that although this is not an abstract class some of the vitual function really need 
	/// to be re-implemented by the child class for them to function properly \n
	/// please refer to the documentation on creating custom attribute 
	class AEWidget : public QWidget
	{
		Q_OBJECT
		
	public:
		
		//! @name Constructors , Desctructors and Create function
		//@{
		/// default contructor take the descriotion info from the Inputs VariantMap 	
		AEWidget( FabricSplice::DGPort port , QWidget* parent = NULL);
		~AEWidget();

		/// creation function
		static AEWidget * create( FabricSplice::DGPort port, QWidget* parent = NULL);
		//@}

		//! @name Default Layout and Label Utils
		//@{
		/// basic layout with name/label and place for the control
		QHBoxLayout * createLabelControlLayout();
		/// get the display label based on the rules : label key first if not present name key ( name is mandatory )
		QString getLabel();
				
		/// when the widget is used in compound or array it is sometime useful
		/// to access the QLabel to change a little its text or shape
		QLabel * getDefaultLabel() {return m_defaultLabelWidget;}
    
    // returns the wrapped Port
    FabricSplice::DGPort getPort() { return m_port; }

    // sets the wrapped Port
    virtual void setPort(FabricSplice::DGPort port) { m_port = port; }

		/// we can create a empty on for missing registration
		void missingRegistration();

		/// setter for the extra action , see the m_extraMenuActions docs for more detail
		void	setExtraMenuActions(QList<QAction*> actions) {m_extraMenuActions = actions;}
		
		/// when in silentUpdate mode the signal : uiChanged is not emited when setInternalValue is called
		/// useful when you are already reacting to the environement and dont want ot create a loop ...
		void setSilentUpdate(bool silent) {m_silentUpdate = silent;}
		
	signals:
		/// emited each time the value of a widget changed
		void AEWigetValueChanged(std::string name);
		
		/// emited each time the internal value of a widget changed and the ui need updating
		void AEWigetInternalChanged(std::string name);

	protected slots:
		/// this should get connected to the widgets that are connected to the value of the attribute
		virtual void uiChanged();
		
	private slots:
		/// call when the internal data as change invalidating any cache in the editor
		void valueChanged();
		
	protected:
		/// the menu in case there is presets
		void contextMenuEvent(QContextMenuEvent *event);
		
		/// will go though each widget children that is not
		/// m_defaultLabelWidget and set its enable state to value
		void setEnabledOnChildren(bool value);

    /// build the correct RTVal based on array or non array
    FabricCore::RTVal rtValConstruct(const char * dataType);

    /// returns the array size of an rtval
    unsigned int rtValGetArraySize(const FabricCore::RTVal value);

    /// returns an element of an rtval array
    FabricCore::RTVal rtValGetArrayElement(const FabricCore::RTVal value, unsigned int index);

    /// sets the array size of an rtval
    void rtValSetArraySize(FabricCore::RTVal & value, unsigned int size);

    /// returns an element of an rtval array
    void rtValSetArrayElement(FabricCore::RTVal & value, unsigned int index, const FabricCore::RTVal & element);

    /// returns an option of a port
    float getPortOption(const char * key, float defaultValue);
		
	protected:

		/// default label Widget
		QLabel * m_defaultLabelWidget;
		
		/// this is the name for this attributes
		std::string m_attributeName;
		
		// parameter that contains all specific information.
		FabricSplice::DGPort m_port;
		
		/// a List of action to be added to each Context Menu
		/// this enable external object to add their action to the Menu
		QList<QAction *> m_extraMenuActions;
				
		/// dictate is the uiUpdate are silenced 
		/// usefull when you want to update a UI without any further action being triggered
		bool m_silentUpdate;
	};
	
	
};


#endif
