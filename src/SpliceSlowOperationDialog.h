#ifndef __SLOWOPERATIONDIALOG_H__
#define __SLOWOPERATIONDIALOG_H__

#include <QtGui/QDialog>
#include <QtGui/QLabel>

namespace FabricSplice {

  class SpliceSlowOperationDialog : public QDialog
  {
    Q_OBJECT

  public:

    SpliceSlowOperationDialog(
      QWidget * parent
      );

  public slots:

    void display( char const *descCStr );

  private:

    QLabel *m_label;
  };
};

#endif // __SLOWOPERATIONDIALOG_H__
