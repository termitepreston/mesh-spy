#ifndef SKYBOX_H
#define SKYBOX_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>

class Skybox : protected QOpenGLExtraFunctions
{
public:
  Skybox ();
  ~Skybox ();

  void init ();
  void render (const QMatrix4x4 &view, const QMatrix4x4 &projection);
  unsigned int
  getTextureId () const
  {
    return m_hdrTexture;
  }

private:
  void loadHDR (const char *path);
  void initCube ();

  unsigned int m_hdrTexture;
  unsigned int m_vao;
  unsigned int m_vbo;
  QOpenGLShaderProgram *m_shader;
};

#endif // SKYBOX_H
