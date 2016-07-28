#ifndef __AEITEM_H__
#define __AEITEM_H__

#include <QVariant>
#include <FabricCore.h>
#include <FabricSplice.h>

/// \brief Simple wrapper around a FabricSplice::DGPort to expose it to Qt
struct QDGPort
{
	FabricSplice::DGPort port;
};

Q_DECLARE_METATYPE(QDGPort)

namespace FabricSplice
{

  /// \brief basic Model item that hold an AE Description VariantMap
  class AEItem
  {
  	public:
  		AEItem(FabricSplice::DGPort port, AEItem *parent = 0);
  		virtual ~AEItem();
  	
  		void appendChild(AEItem *child);
  		
  		virtual AEItem *child(int row);
  		virtual int childCount() const;
  		int columnCount() const;
  		
  		QVariant data(int column) const;
  		QVariant data(int column , int role) const;
  		int row() const;
  		AEItem *parent();
  	
  		FabricSplice::DGPort getPort() { return m_port;}
  		
  	private:
  		QList<AEItem*> childItems;
  		
  		mutable FabricSplice::DGPort m_port;
  		AEItem *m_parentItem;
  	
  };

};

#endif // __AEITEM_H__
