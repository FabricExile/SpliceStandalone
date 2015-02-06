#include "SpliceSlowOperationDialog.h"

#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>

namespace FabricSplice
{
  SpliceSlowOperationDialog::SpliceSlowOperationDialog(
    QWidget * parent
    )
    : QDialog(
      parent,
      Qt::SplashScreen
      )
  {
    QHBoxLayout *layout = new QHBoxLayout( this );

    m_label = new QLabel( this );
    layout->addWidget( m_label );
  }

  void SpliceSlowOperationDialog::display( char const *descCStr )
  {
    // fprintf( stderr, "ST %s\n", descCStr );

    QString desc;
    if ( descCStr && *descCStr )
    {
      desc += "Fabric Core: ";
      desc += descCStr;
      desc += "...";
    }
    m_label->setText( desc );

    if ( !desc.isEmpty() )
      show();
    else
      hide();
  }
};