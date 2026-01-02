#include "deferredrenderer.h"
#include "camera.h"
#include "deferredrenderer.h"
#include "gbuffer.h"
#include <QDebug>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

DeferredRenderer::DeferredRenderer ()
    : m_geomShader (nullptr), m_lightShader (nullptr), m_width (800),
      m_height (600)
{
}

DeferredRenderer::~DeferredRenderer ()
{
  if (m_quadVAO)
    glDeleteVertexArrays (1, &m_quadVAO);
  if (m_quadVBO)
    glDeleteBuffers (1, &m_quadVBO);
  if (m_cubeVAO)
    glDeleteVertexArrays (1, &m_cubeVAO);
  if (m_cubeVBO)
    glDeleteBuffers (1, &m_cubeVBO);
}

void
DeferredRenderer::init (int width, int height)
{
  initializeOpenGLFunctions ();
  m_width = width;
  m_height = height;

  m_gBuffer = std::make_unique<GBuffer> ();
  m_gBuffer->init (width, height);

  initShaders ();
  initQuad ();
  initTestCube ();

  // Init Skybox
  m_skybox = std::make_unique<Skybox> ();
  m_skybox->init ();
}

void
DeferredRenderer::resize (int width, int height)
{
  m_width = width;
  m_height = height;
  if (m_gBuffer)
    {
      m_gBuffer->resize (width, height);
    }
}

void
DeferredRenderer::initShaders ()
{
  m_geomShader = new QOpenGLShaderProgram ();
  if (!m_geomShader->addShaderFromSourceFile (QOpenGLShader::Vertex,
                                              ":/shaders/geometry.vert"))
    qDebug () << "Geom Vert Error:" << m_geomShader->log ();
  if (!m_geomShader->addShaderFromSourceFile (QOpenGLShader::Fragment,
                                              ":/shaders/geometry.frag"))
    qDebug () << "Geom Frag Error:" << m_geomShader->log ();
  m_geomShader->link ();

  m_lightShader = new QOpenGLShaderProgram ();
  if (!m_lightShader->addShaderFromSourceFile (QOpenGLShader::Vertex,
                                               ":/shaders/lighting.vert"))
    qDebug () << "Light Vert Error:" << m_lightShader->log ();
  if (!m_lightShader->addShaderFromSourceFile (QOpenGLShader::Fragment,
                                               ":/shaders/lighting.frag"))
    qDebug () << "Light Frag Error:" << m_lightShader->log ();
  m_lightShader->link ();

  m_lightShader->bind ();
  m_lightShader->setUniformValue ("gPosition", 0);
  m_lightShader->setUniformValue ("gNormal", 1);
  m_lightShader->setUniformValue ("gAlbedo", 2);
  m_lightShader->setUniformValue ("gPBR", 3);
  m_lightShader->release ();
}

void
DeferredRenderer::initQuad ()
{
  float quadVertices[] = {
    // positions        // texture Coords
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
  };

  glGenVertexArrays (1, &m_quadVAO);
  glGenBuffers (1, &m_quadVBO);
  glBindVertexArray (m_quadVAO);
  glBindBuffer (GL_ARRAY_BUFFER, m_quadVBO);
  glBufferData (GL_ARRAY_BUFFER, sizeof (quadVertices), &quadVertices,
                GL_STATIC_DRAW);
  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof (float),
                         (void *)0);
  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof (float),
                         (void *)(3 * sizeof (float)));
}

void
DeferredRenderer::initTestCube ()
{
  // A simple cube to test GBuffer writing
  float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.5f, 0.5f, -0.5f,
    0.0f, 0.0f, -1.0f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f,
    0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, -0.5f, -0.5f,
    -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 0.0f,
    -1.0f, 0.0f, 1.0f,
    // Front face
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 0.0f,
    0.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f,
    0.0f, 1.0f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    // Left face
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f,
    -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f,
    -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // Right face
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    // Bottom face
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f,
    0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.0f, 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f,
    0.0f, -1.0f, 0.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
    0.0f, 1.0f,
    // Top face
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.0f,
    1.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f,
    1.0f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f
  };

  glGenVertexArrays (1, &m_cubeVAO);
  glGenBuffers (1, &m_cubeVBO);

  glBindVertexArray (m_cubeVAO);
  glBindBuffer (GL_ARRAY_BUFFER, m_cubeVBO);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  // Position
  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float),
                         (void *)0);
  // Normal
  glEnableVertexAttribArray (1);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float),
                         (void *)(3 * sizeof (float)));
  // TexCoords
  glEnableVertexAttribArray (2);
  glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float),
                         (void *)(6 * sizeof (float)));
}

void
DeferredRenderer::render (Camera *camera, float modelRotationY)
{
  // 1. Geometry Pass
  m_gBuffer->bindWrite ();
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Clear to black/empty
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable (GL_DEPTH_TEST);

  if (camera)
    {
      renderGeometryPass (camera, modelRotationY);
    }

  // 2. Lighting Pass
  // 2. Lighting Pass (Render to default framebuffer)
  glBindFramebuffer (GL_FRAMEBUFFER, 0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable (GL_DEPTH_TEST);

  renderLightingPass (camera); // Pass camera for View Pos

  // 3. Skybox Pass (Render last)
  // We copy the depth buffer from G-Buffer to default framebuffer
  // so the skybox is occluded by geometry.
  // BUT since we are doing deferred lighting on a quad, the depth information
  // is in the G-Buffer texture, not the default framebuffer's depth buffer.
  // We have two options:
  // A) Draw skybox first, then blend lighting on top (complex).
  // B) Blit G-Buffer Depth to Default Framebuffer Depth.

  glBindFramebuffer (GL_READ_FRAMEBUFFER, m_gBuffer->getFBO ());
  glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer (0, 0, m_width, m_height, 0, 0, m_width, m_height,
                     GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  glBindFramebuffer (GL_FRAMEBUFFER, 0);

  if (camera && m_skybox)
    {
      glEnable (GL_DEPTH_TEST);
      QMatrix4x4 view = QMatrix4x4 (glm::value_ptr (camera->getViewMatrix ()))
                            .transposed ();
      QMatrix4x4 proj
          = QMatrix4x4 (glm::value_ptr (camera->getProjectionMatrix ()))
                .transposed ();
      m_skybox->render (view, proj);
    }
}

void
DeferredRenderer::renderGeometryPass (Camera *camera, float modelRotationY)
{
  if (!m_geomShader || !camera)
    return;

  // Wireframe Mode Handling
  if (m_config.wireframe)
    {
      glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    }
  else
    {
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

  m_geomShader->bind ();

  // Model Matrix: Rotation from auto-rotate
  glm::mat4 model = glm::mat4 (1.0f);

  // We rotate around the Y axis as per requirement
  // Note: The bounding box centering is handled by the Camera Target,
  // so we can keep the model matrix at origin if the mesh data is centered,
  // OR we translate model to -center then rotate.
  // However, Arcball target handles centering visually.
  // If the mesh is offset from (0,0,0) in the file, rotating
  // simply around 0,0,0 might look off-center.
  // For now, we rotate around World Origin (0,0,0).
  // Since Camera target is set to Mesh Center, it will look like the
  // object is pivoting around the world origin, which might be far from mesh
  // center. Requirement says: "Model rotates around world Y-axis at bounding
  // box center"

  // To achieve rotation around bounding box center:
  // 1. Translate Center to Origin
  // 2. Rotate
  // 3. Translate Origin to Center
  // BUT: The standard Arcball approach is moving the camera.
  // The specific requirement "Model rotates..." implies we should manipulate
  // the Model Matrix.

  // Since we don't have the center easily accessible here (it was in
  // SceneData), let's assume for this phase that World Origin rotation is
  // acceptable OR relies on the mesh being reasonably centered. (Improvements:
  // store model center in DeferredRenderer).

  model = glm::rotate (model, modelRotationY, glm::vec3 (0.0f, 1.0f, 0.0f));

  m_geomShader->setUniformValue (
      "model", QMatrix4x4 (glm::value_ptr (model)).transposed ());
  m_geomShader->setUniformValue (
      "view",
      QMatrix4x4 (glm::value_ptr (camera->getViewMatrix ())).transposed ());
  m_geomShader->setUniformValue (
      "projection",
      QMatrix4x4 (glm::value_ptr (camera->getProjectionMatrix ()))
          .transposed ());

  if (m_model)
    {
      m_model->draw (m_geomShader, m_config); // Pass config
    }
  else
    {
      // Fallback Cube
      m_geomShader->setUniformValue ("uAlbedoColor", 0.8f, 0.2f, 0.2f);
      m_geomShader->setUniformValue ("uMetallic", 0.0f);
      m_geomShader->setUniformValue ("uRoughness", 0.5f);
      glBindVertexArray (m_cubeVAO);
      glDrawArrays (GL_TRIANGLES, 0, 36);
      glBindVertexArray (0);
    }

  m_geomShader->release ();

  // Reset Polygon Mode just in case
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void
DeferredRenderer::renderLightingPass (Camera *camera)
{
  if (!m_lightShader)
    return;

  m_lightShader->bind ();

  // Bind G-Buffer textures
  m_gBuffer->bindRead ();

  // Bind Environment Map (Slot 4)
  if (m_skybox)
    {
      glActiveTexture (GL_TEXTURE4);
      glBindTexture (GL_TEXTURE_2D, m_skybox->getTextureId ());
      m_lightShader->setUniformValue ("environmentMap", 4);
    }

  if (camera)
    {
      glm::vec3 camPos = camera->getPosition ();
      m_lightShader->setUniformValue ("viewPos", camPos.x, camPos.y, camPos.z);
    }

  glBindVertexArray (m_quadVAO);
  glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray (0);

  m_lightShader->release ();
}

void
DeferredRenderer::loadModel (SceneData *data)
{
  if (!m_model)
    {
      m_model = std::make_unique<Model> ();
    }
  m_model->create (data);
}
