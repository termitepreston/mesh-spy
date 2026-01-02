#ifndef MODEL_H
#define MODEL_H

#include "meshdata.h"
#include "renderconfig.h"
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <memory>
#include <vector>

struct GLMesh
{
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;
  unsigned int indexCount;
  int materialIndex;
};

struct GLTexture
{
  unsigned int id;
  bool isValid;
};

class Model : protected QOpenGLExtraFunctions
{
public:
  Model ();
  ~Model ();

  void create (SceneData *data);
  void draw (QOpenGLShaderProgram *shader, const RenderConfig &config);

private:
  std::vector<GLMesh> m_glMeshes;
  std::vector<GLTexture> m_glTextures;
  std::vector<MaterialData> m_materials;

  // We keep track to delete them
  void clear ();
};

#endif // MODEL_H
