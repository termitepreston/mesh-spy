#include "skybox.h"
#include <QDebug>
#include <QFile>

#ifdef STB_IMAGE_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

Skybox::Skybox () : m_hdrTexture (0), m_vao (0), m_vbo (0), m_shader (nullptr)
{
}

Skybox::~Skybox ()
{
  if (m_vao)
    glDeleteVertexArrays (1, &m_vao);
  if (m_vbo)
    glDeleteBuffers (1, &m_vbo);
  if (m_hdrTexture)
    glDeleteTextures (1, &m_hdrTexture);
  delete m_shader;
}

void
Skybox::init ()
{
  initializeOpenGLFunctions ();

  // Load Shader
  m_shader = new QOpenGLShaderProgram ();
  m_shader->addShaderFromSourceFile (QOpenGLShader::Vertex,
                                     ":/shaders/skybox.vert");
  m_shader->addShaderFromSourceFile (QOpenGLShader::Fragment,
                                     ":/shaders/skybox.frag");
  m_shader->link ();

  // Init Cube Geometry
  initCube ();

  // Load Texture
  // Copy resource to temp file because stb_image needs a file path or buffer.
  // Reading directly from QResource via buffer is better.
  QFile file (":/textures/rogland_clear_night_2k.hdr");
  if (file.open (QIODevice::ReadOnly))
    {
      QByteArray data = file.readAll ();

      int width, height, nrComponents;
      stbi_set_flip_vertically_on_load (true);
      float *dataPtr = stbi_loadf_from_memory (
          reinterpret_cast<const unsigned char *> (data.constData ()),
          data.size (), &width, &height, &nrComponents, 0);

      stbi_set_flip_vertically_on_load (false);

      if (dataPtr)
        {
          glGenTextures (1, &m_hdrTexture);
          glBindTexture (GL_TEXTURE_2D, m_hdrTexture);
          glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                        GL_FLOAT, dataPtr);

          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          // Generate mips for roughness approximation
          glGenerateMipmap (GL_TEXTURE_2D);

          stbi_image_free (dataPtr);
          qDebug () << "HDR Skybox loaded successfully.";
        }
      else
        {
          qDebug () << "Failed to load HDR image.";
        }
      file.close ();
    }
  else
    {
      qDebug () << "Could not open HDR resource.";
    }
}

void
Skybox::render (const QMatrix4x4 &view, const QMatrix4x4 &projection)
{
  if (!m_shader || !m_hdrTexture)
    return;

  glDepthFunc (GL_LEQUAL); // Allow skybox to pass at depth 1.0
  m_shader->bind ();

  m_shader->setUniformValue ("view", view);
  m_shader->setUniformValue ("projection", projection);
  m_shader->setUniformValue ("environmentMap", 0);

  glActiveTexture (GL_TEXTURE0);
  glBindTexture (GL_TEXTURE_2D, m_hdrTexture);

  glBindVertexArray (m_vao);
  glDrawArrays (GL_TRIANGLES, 0, 36);
  glBindVertexArray (0);

  m_shader->release ();
  glDepthFunc (GL_LESS);
}

void
Skybox::initCube ()
{
  float vertices[]
      = { // positions
          -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
          1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

          -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
          -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

          1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

          -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

          -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

          -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
          1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f
        };

  glGenVertexArrays (1, &m_vao);
  glGenBuffers (1, &m_vbo);
  glBindVertexArray (m_vao);
  glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), &vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                         (void *)0);
}
