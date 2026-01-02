#ifndef GLVIEWWIDGET_H
#define GLVIEWWIDGET_H

#include "meshdata.h"
#include "renderconfig.h"
#include <QMouseEvent>
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <QWheelEvent>
#include <memory>

class DeferredRenderer;
class Camera;

class GLViewWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
  Q_OBJECT

public:
  explicit GLViewWidget (QWidget *parent = nullptr);
  ~GLViewWidget () override;
  void loadModel (SceneData *data);
  void setMaterialSettings (const RenderConfig &config);

protected:
  void initializeGL () override;
  void resizeGL (int w, int h) override;
  void paintGL () override;

  // Input Events
  void mousePressEvent (QMouseEvent *event) override;
  void mouseMoveEvent (QMouseEvent *event) override;
  void mouseReleaseEvent (QMouseEvent *event) override;
  void wheelEvent (QWheelEvent *event) override;

private:
  void handleInteraction ();

  QTimer m_renderTimer;
  QTimer m_idleTimer; // Detects 3 seconds of inactivity

  std::unique_ptr<DeferredRenderer> m_renderer;
  std::unique_ptr<Camera> m_camera;

  // Input State
  QPoint m_lastMousePos;
  bool m_isRotating = false;
  bool m_isPanning = false;

  // Auto-rotation State
  bool m_autoRotateActive = true; // Starts active
  float m_modelRotationAngle = 0.0f;
};

#endif // GLVIEWWIDGET_H
