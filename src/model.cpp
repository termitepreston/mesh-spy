#include "model.h"
#include <QDebug>

Model::Model () { initializeOpenGLFunctions (); }

Model::~Model () { clear (); }

void
Model::clear ()
{
  for (auto &mesh : m_glMeshes)
    {
      glDeleteVertexArrays (1, &mesh.vao);
      glDeleteBuffers (1, &mesh.vbo);
      glDeleteBuffers (1, &mesh.ebo);
    }
  m_glMeshes.clear ();

  for (auto &tex : m_glTextures)
    {
      if (tex.isValid)
        glDeleteTextures (1, &tex.id);
    }
  m_glTextures.clear ();
}

void
Model::create (SceneData *data)
{
  clear ();
  if (!data)
    return;

  // 1. Upload Textures
  for (const auto &texData : data->textures)
    {
      GLTexture tex;
      tex.isValid = false;

      if (!texData.pixels.empty ())
        {
          glGenTextures (1, &tex.id);
          glBindTexture (GL_TEXTURE_2D, tex.id);

          GLenum format = GL_RGBA;
          if (texData.components == 1)
            format = GL_RED;
          else if (texData.components == 3)
            format = GL_RGB;
          else if (texData.components == 4)
            format = GL_RGBA;

          glTexImage2D (GL_TEXTURE_2D, 0, format, texData.width,
                        texData.height, 0, format, GL_UNSIGNED_BYTE,
                        texData.pixels.data ());
          glGenerateMipmap (GL_TEXTURE_2D);

          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           GL_LINEAR_MIPMAP_LINEAR);
          glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          tex.isValid = true;
        }
      m_glTextures.push_back (tex);
    }

  // 2. Upload Materials
  m_materials = data->materials;

  // 3. Upload Meshes
  for (const auto &subMesh : data->meshes)
    {
      GLMesh mesh;
      mesh.indexCount = (unsigned int)subMesh.indices.size ();
      mesh.materialIndex = subMesh.materialIndex;

      glGenVertexArrays (1, &mesh.vao);
      glGenBuffers (1, &mesh.vbo);
      glGenBuffers (1, &mesh.ebo);

      glBindVertexArray (mesh.vao);

      glBindBuffer (GL_ARRAY_BUFFER, mesh.vbo);
      glBufferData (GL_ARRAY_BUFFER,
                    subMesh.vertices.size () * sizeof (Vertex),
                    subMesh.vertices.data (), GL_STATIC_DRAW);

      glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
      glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                    subMesh.indices.size () * sizeof (unsigned int),
                    subMesh.indices.data (), GL_STATIC_DRAW);

      // Pos
      glEnableVertexAttribArray (0);
      glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
                             (void *)0);
      // Norm
      glEnableVertexAttribArray (1);
      glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex),
                             (void *)offsetof (Vertex, normal));
      // Tex
      glEnableVertexAttribArray (2);
      glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex),
                             (void *)offsetof (Vertex, texCoords));

      glBindVertexArray (0);
      m_glMeshes.push_back (mesh);
    }
}

void
Model::draw (QOpenGLShaderProgram *shader)
{
  for (const auto &mesh : m_glMeshes)
    {
      // Apply Material Props
      if (mesh.materialIndex >= 0 && mesh.materialIndex < m_materials.size ())
        {
          const MaterialData &mat = m_materials[mesh.materialIndex];
          shader->setUniformValue ("uAlbedoColor", mat.baseColorFactor.x,
                                   mat.baseColorFactor.y,
                                   mat.baseColorFactor.z);
          shader->setUniformValue ("uMetallic", mat.metallicFactor);
          shader->setUniformValue ("uRoughness", mat.roughnessFactor);
          // Texture binding to be implemented fully in Material phase,
          // for now we rely on color/factors.
        }

      glBindVertexArray (mesh.vao);
      glDrawElements (GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
      glBindVertexArray (0);
    }
}
