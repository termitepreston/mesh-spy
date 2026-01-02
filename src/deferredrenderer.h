#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <memory>

#include "model.h"

class GBuffer;

class DeferredRenderer : protected QOpenGLExtraFunctions
{
public:
  DeferredRenderer ();
  ~DeferredRenderer ();

  void init (int width, int height);
  void resize (int width, int height);
  void render (); // Main render entry point

  void loadModel (SceneData *data);

private:
  void initShaders ();
  void initQuad ();     // For lighting pass
  void initTestCube (); // Temporary for Phase 2

  // Passes
  void renderGeometryPass ();
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
};

#endif // DEFERREDRENDERER_H
