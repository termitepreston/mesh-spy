#include "glviewwidget.h"
#include "deferredrenderer.h"
#include <QDebug>

GLViewWidget::GLViewWidget (QWidget *parent) : QOpenGLWidget (parent)
{
  // Setup 60 FPS Render Loop
  connect (&m_renderTimer, &QTimer::timeout, this,
           QOverload<>::of (&GLViewWidget::update));
  m_renderTimer.start (16);
}

GLViewWidget::~GLViewWidget ()
{
  makeCurrent ();
  m_renderer.reset (); // Cleanup OpenGL resources in renderer
  doneCurrent ();
}

void
GLViewWidget::initializeGL ()
{
  initializeOpenGLFunctions ();

  qDebug () << "OpenGL Version:" << (const char *)glGetString (GL_VERSION);

  // Create Renderer
  m_renderer = std::make_unique<DeferredRenderer> ();
  m_renderer->init (width (), height ());
}

void
GLViewWidget::resizeGL (int w, int h)
{
  if (m_renderer)
    {
      m_renderer->resize (w, h);
    }
}

void
GLViewWidget::paintGL ()
{
  if (m_renderer)
    {
      m_renderer->render ();
    }
}

void
GLViewWidget::loadModel (SceneData *data)
{
  makeCurrent ();
  if (m_renderer)
    {
      m_renderer->loadModel (data);
    }
  // Clean up the CPU data as the renderer copied what it needed to GPU,
  // unless we want to keep it for bounding box calcs later.
  // For now, let's delete it to prevent leaks.
  delete data;
  doneCurrent ();
  update ();
}
