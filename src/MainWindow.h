#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSpinBox>
#include <QtGui/QToolButton>
#include <QtGui/QCheckBox>
#include <QtGui/QTableWidget>
#include <QtGui/QSlider>
#include <QtGui/QGroupBox>
#include <QtCore/QTime>
#include "SpliceGraphWrapper.h"

#include "Widgets/LogWidget.h"
#include "Widgets/GLWidget.h"
#include "ManipulationTool.h"
#include "Widgets/KLEditor.h"
#include "Widgets/AttributeEditorWidget.h"

#include "utility.h"

namespace FabricSplice
{
  class GLWidget;
  class TimeSliderWidget;
  class MainWindow;

  class MainWindowKeyFilter : public QObject
  {
  public:
    MainWindowKeyFilter(MainWindow * window);
    ~MainWindowKeyFilter( ) {};

    bool eventFilter(QObject* object,QEvent* event);

  protected:

    MainWindow * m_window;
  };

  class MainWindow : public QMainWindow
  {
    friend class MainWindowKeyFilter;
    
    Q_OBJECT

  public:

    MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = Qt::CustomizeWindowHint );
    ~MainWindow() {};

    // make sure the widgetRedraw;
    void redraw();
    void displayMessage(std::string message);

  public slots:
    void updateViews();

    void toggleManipulation();

    void attributeChanged( QSpliceGraphWrapper wrapper, std::string attributeName );
    void timeChanged(int frame);
    void setGlViewEnabled(bool enable);
    void updateStatusBar(bool force = false);
    void setStatusBarText(QString caption);

    void showKLEditor();
    void showLogWindow();
    void showAttributeEditor();

    void destroyedKLEditor(QObject * obj = 0);
    void destroyedLogWindow(QObject * obj = 0);
    void destroyedAttributeEditor(QObject * obj = 0);

    void displayAsWireFrame();
    void displayAsShaded();

  protected:

    void bringToFront(QWidget * widget);

    GLWidget *  m_glWidget;

    std::vector<AttributeEditorWidget *> m_attributeEditors;
    std::vector<KLEditor *> m_sourceEditors;
    std::vector<LogWidget *> m_logWidgets;

    TimeSliderWidget * m_timeSliderWidget;

    ManipulationTool *  m_manipulatorContext;

    MainWindowKeyFilter * m_eventFilter;

    QStatusBar * m_statusBar;
    QString m_statusBarCaption;
    QTime m_statusBarTimer;
  };
};

#endif // __MAINWINDOW_H__


