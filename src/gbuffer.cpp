#include "gbuffer.h"
#include <QDebug>

GBuffer::GBuffer () : m_fbo (0), m_rboDepth (0), m_width (0), m_height (0)
{
  for (int i = 0; i < 4; i++)
    m_textures[i] = 0;
}

GBuffer::~GBuffer ()
{
  if (m_fbo)
    glDeleteFramebuffers (1, &m_fbo);
  if (m_textures[0])
    glDeleteTextures (4, m_textures);
  if (m_rboDepth)
    glDeleteRenderbuffers (1, &m_rboDepth);
}

bool
GBuffer::init (int width, int height)
{
  initializeOpenGLFunctions ();
  m_width = width;
  m_height = height;

  glGenFramebuffers (1, &m_fbo);
  glBindFramebuffer (GL_FRAMEBUFFER, m_fbo);

  // 1. Position + Depth (RGBA16F)
  glGenTextures (1, &m_textures[0]);
  glBindTexture (GL_TEXTURE_2D, m_textures[0]);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                GL_FLOAT, NULL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                          m_textures[0], 0);

  // 2. Normal + Emissive (RGBA16F)
  glGenTextures (1, &m_textures[1]);
  glBindTexture (GL_TEXTURE_2D, m_textures[1]);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                GL_FLOAT, NULL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                          m_textures[1], 0);

  // 3. Albedo + Alpha (RGBA8)
  glGenTextures (1, &m_textures[2]);
  glBindTexture (GL_TEXTURE_2D, m_textures[2]);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, NULL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                          m_textures[2], 0);

  // 4. PBR: Metal/Rough/AO (RGBA8)
  glGenTextures (1, &m_textures[3]);
  glBindTexture (GL_TEXTURE_2D, m_textures[3]);
  glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                GL_UNSIGNED_BYTE, NULL);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
                          m_textures[3], 0);

  // Draw Buffers
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                                  GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers (4, attachments);

  // Depth/Stencil RBO
  glGenRenderbuffers (1, &m_rboDepth);
  glBindRenderbuffer (GL_RENDERBUFFER, m_rboDepth);
  glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                             GL_RENDERBUFFER, m_rboDepth);

  if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      qDebug () << "GBuffer Framebuffer not complete!";
      return false;
    }

  glBindFramebuffer (GL_FRAMEBUFFER, 0);
  return true;
}

void
GBuffer::resize (int width, int height)
{
  // Simple regeneration for now
  if (m_fbo)
    {
      glDeleteFramebuffers (1, &m_fbo);
      glDeleteTextures (4, m_textures);
      glDeleteRenderbuffers (1, &m_rboDepth);
    }
  init (width, height);
}

void
GBuffer::bindWrite ()
{
  glBindFramebuffer (GL_FRAMEBUFFER, m_fbo);
}

void
GBuffer::bindRead ()
{
  for (unsigned int i = 0; i < 4; i++)
    {
      glActiveTexture (GL_TEXTURE0 + i);
      glBindTexture (GL_TEXTURE_2D, m_textures[i]);
    }
}

unsigned int
GBuffer::getFBO () const
{
  return m_fbo;
}
