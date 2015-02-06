#include "SpliceSlowOperationDialog.h"

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
    m_label->setText( descCStr );
    if ( descCStr && *descCStr )
      show();
    else
      hide();
  }
};