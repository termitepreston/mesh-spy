#ifndef GBUFFER_H
#define GBUFFER_H

#include <QOpenGLExtraFunctions>

class GBuffer : protected QOpenGLExtraFunctions
{
public:
  GBuffer ();
  ~GBuffer ();

  bool init (int width, int height);
  void bindWrite ();
  void bindRead (); // Binds textures to units 0-3
  void resize (int width, int height);

private:
  unsigned int m_fbo;
  unsigned int m_textures[4]; // Pos, Norm, Albedo, PBR
  unsigned int m_rboDepth;    // Depth/Stencil

  int m_width;
  int m_height;
};

#endif // GBUFFER_H
