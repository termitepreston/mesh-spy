#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <memory>

#include "model.h"
#include "renderconfig.h"

class GBuffer;
class Camera;

class DeferredRenderer : protected QOpenGLExtraFunctions
{
public:
  DeferredRenderer ();
  ~DeferredRenderer ();

  void init (int width, int height);
  void resize (int width, int height);
  void render (Camera *camera,
               float modelRotationY); // Main render entry point
  void
  setConfig (const RenderConfig &config)
  {
    m_config = config;
  } // New setter

  void loadModel (SceneData *data);

private:
  void initShaders ();
  void initQuad ();     // For lighting pass
  void initTestCube (); // Temporary for Phase 2

  // Passes
  void renderGeometryPass (Camera *camera, float modelRotationY);
  void renderLightingPass ();

  std::unique_ptr<GBuffer> m_gBuffer;

  QOpenGLShaderProgram *m_geomShader;
  QOpenGLShaderProgram *m_lightShader;

  // Full Screen Quad Resources
  unsigned int m_quadVAO = 0;
  unsigned int m_quadVBO = 0;

  // Test Cube Resources
  unsigned int m_cubeVAO = 0;
  unsigned int m_cubeVBO = 0;

  int m_width;
  int m_height;

  std::unique_ptr<Model> m_model;

  RenderConfig m_config; // Store settings
};

#endif // DEFERREDRENDERER_H
