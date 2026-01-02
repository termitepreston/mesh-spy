#ifndef GLVIEWWIDGET_H
#define GLVIEWWIDGET_H

#include "meshdata.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLWidget>
#include <QTimer>
#include <memory>

class DeferredRenderer;

class GLViewWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
  Q_OBJECT

public:
  explicit GLViewWidget (QWidget *parent = nullptr);
  ~GLViewWidget () override;
  void loadModel (SceneData *data);

protected:
  void initializeGL () override;
  void resizeGL (int w, int h) override;
  void paintGL () override;

private:
  QTimer m_renderTimer;
  std::unique_ptr<DeferredRenderer> m_renderer;
};

#endif // GLVIEWWIDGET_H
