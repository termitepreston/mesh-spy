#include "glviewwidget.h"
#include "camera.h"
#include "deferredrenderer.h"
#include <QDebug>

GLViewWidget::GLViewWidget (QWidget *parent) : QOpenGLWidget (parent)
{
  // Focus policy needed for keyboard/mouse
  setFocusPolicy (Qt::StrongFocus);

  // Setup 60 FPS Render Loop
  connect (&m_renderTimer, &QTimer::timeout, this,
           QOverload<>::of (&GLViewWidget::update));
  m_renderTimer.start (16);

  // Idle Timer (3 seconds)
  m_idleTimer.setSingleShot (true);
  m_idleTimer.setInterval (3000);
  connect (&m_idleTimer, &QTimer::timeout, this,
           [this] () { m_autoRotateActive = true; });
}

GLViewWidget::~GLViewWidget ()
{
  makeCurrent ();
  m_renderer.reset ();
  m_camera.reset ();
  doneCurrent ();
}

void
GLViewWidget::initializeGL ()
{
  initializeOpenGLFunctions ();
  m_camera = std::make_unique<Camera> ();
  m_renderer = std::make_unique<DeferredRenderer> ();
  m_renderer->init (width (), height ());
}

void
GLViewWidget::resizeGL (int w, int h)
{
  if (m_renderer)
    m_renderer->resize (w, h);
  if (m_camera)
    m_camera->setViewportSize (w, h);
}

void
GLViewWidget::paintGL ()
{
  // Update Auto-rotation logic
  if (m_autoRotateActive)
    {
      // 30 degrees per second.
      // 60 FPS -> 16ms/frame -> ~0.016s.
      // 30 deg * 0.016s = 0.5 degrees per frame = ~0.0087 radians
      m_modelRotationAngle += 0.0087f;
    }

  if (m_renderer && m_camera)
    {
      m_renderer->render (m_camera.get (), m_modelRotationAngle);
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

  // Auto-center camera on model
  if (m_camera && data)
    {
      glm::vec3 center = (data->minBounds + data->maxBounds) * 0.5f;
      float size = glm::length (data->maxBounds - data->minBounds);

      m_camera->setTarget (center);
      m_camera->setDistance (size * 1.5f); // Fit to view
    }

  delete data;
  doneCurrent ();

  // Reset rotation angle
  m_modelRotationAngle = 0.0f;
  m_autoRotateActive = true;
}

void
GLViewWidget::handleInteraction ()
{
  m_autoRotateActive = false;
  m_idleTimer.start (); // Restart the 3 second countdown
}

void
GLViewWidget::mousePressEvent (QMouseEvent *event)
{
  m_lastMousePos = event->pos ();
  if (event->button () == Qt::LeftButton)
    m_isRotating = true;
  if (event->button () == Qt::RightButton)
    m_isPanning = true;
  handleInteraction ();
}

void
GLViewWidget::mouseMoveEvent (QMouseEvent *event)
{
  if (!m_camera)
    return;

  int dx = event->pos ().x () - m_lastMousePos.x ();
  int dy = event->pos ().y () - m_lastMousePos.y ();

  if (m_isRotating)
    {
      // Sensitivity factor
      m_camera->rotate (dx * 0.01f, dy * 0.01f);
      handleInteraction ();
    }

  if (m_isPanning)
    {
      m_camera->pan (dx, dy);
      handleInteraction ();
    }

  m_lastMousePos = event->pos ();
  update (); // Request redraw immediately for responsiveness
}

void
GLViewWidget::mouseReleaseEvent (QMouseEvent *event)
{
  if (event->button () == Qt::LeftButton)
    m_isRotating = false;
  if (event->button () == Qt::RightButton)
    m_isPanning = false;
}

void
GLViewWidget::wheelEvent (QWheelEvent *event)
{
  if (m_camera)
    {
      // QWheelEvent delta is 120 per click usually
      float zoomAmount = event->angleDelta ().y () * 0.01f;
      m_camera->zoom (zoomAmount);
      handleInteraction ();
      update ();
    }
}
