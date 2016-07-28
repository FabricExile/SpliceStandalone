#ifndef __GLWIDGET_H__
#define __GLWIDGET_H__

// #include <GL/glew.h>

#include <QtCore>
#include <QMouseEvent>
#include <QMainWindow>
#include <QImage>
#include <QDialog>
#include <QGLWidget>
#include <QTime>

#include <FabricSplice.h>
#include <FabricCore.h>

namespace FabricSplice
{
  class GLWidget : public QGLWidget
  {
  	Q_OBJECT

  public:

  	GLWidget(QGLFormat format, QWidget *parent = NULL);
  	virtual ~GLWidget();

    void initialize();

  	FabricCore::RTVal getInlineViewport();
    bool manipulateCamera(QEvent *event);

    void resetRTVals();

    void enableRedraw(bool enable = true) { m_redrawEnabled = enable; }
    bool isRedrawEnabled() { return m_redrawEnabled; }
    void setTime(float time);
    void setWireFrame(bool wireFrame);
    void toggleGLFullScreen(int screenIndex = -1);
    bool isGLFullScreen() { return m_fullScreenDialog != NULL; }
    
    /// returns the real frames per second of this TimeSlider
    double fps() const { return m_fps; }

  signals:

    void redrawn();

  protected:

    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    FabricCore::RTVal m_drawing;
    FabricCore::RTVal m_camera;
    FabricCore::RTVal m_cameraManipulator;
    FabricCore::RTVal m_viewport;
    FabricCore::RTVal m_drawContext;

    bool m_requiresInitialize;
    bool m_requiresResize;
    bool m_resizeEnabled;

    QTime m_fpsTimer;
    double m_fps;
    double m_fpsStack[16];
    bool m_redrawEnabled;
    bool m_painting;
    unsigned int m_width;
    unsigned int m_height;

    QWidget * m_prevParent;
    QDialog * m_fullScreenDialog;

  };
};

#endif // __GLWIDGET_H__
