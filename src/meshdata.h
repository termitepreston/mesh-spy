#ifndef MESHDATA_H
#define MESHDATA_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct TextureData
{
  std::vector<unsigned char> pixels;
  int width;
  int height;
  int components;
  std::string name;
};

struct MaterialData
{
  glm::vec4 baseColorFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
  float metallicFactor = 1.0f;
  float roughnessFactor = 1.0f;

  // -1 means no texture
  int baseColorIndex = -1;
  int metallicRoughnessIndex = -1;
  int normalIndex = -1;
};

struct SubMesh
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  int materialIndex = 0;
};

struct SceneData
{
  std::vector<SubMesh> meshes;
  std::vector<MaterialData> materials;
  std::vector<TextureData> textures;
  bool success = false;
  std::string error;

  // Bounding box
  glm::vec3 minBounds = glm::vec3 (FLT_MAX);
  glm::vec3 maxBounds = glm::vec3 (-FLT_MAX);
};

#endif // MESHDATA_H
