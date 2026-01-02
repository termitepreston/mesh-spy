#include "gltfloader.h"

// Define implementation only here
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <QDebug>
#include <glm/gtc/type_ptr.hpp>

void
GLTFLoader::process (QString filepath)
{
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool ret = loader.LoadBinaryFromFile (&model, &err, &warn,
                                        filepath.toStdString ());

  if (!warn.empty ())
    {
      qWarning () << "GLTF Warning: " << QString::fromStdString (warn);
    }

  if (!ret)
    {
      emit error (QString::fromStdString (err));
      return;
    }

  SceneData *sceneData = new SceneData ();
  sceneData->success = true;

  // Initialize bounds
  glm::vec3 globalMin (FLT_MAX);
  glm::vec3 globalMax (-FLT_MAX);

  // 1. Load Textures
  for (const auto &tex : model.textures)
    {
      if (tex.source > -1 && tex.source < model.images.size ())
        {
          tinygltf::Image &image = model.images[tex.source];
          TextureData texData;
          texData.width = image.width;
          texData.height = image.height;
          texData.components = image.component;
          texData.pixels = image.image; // Copy data
          texData.name = image.name;
          sceneData->textures.push_back (texData);
        }
      else
        {
          sceneData->textures.push_back (TextureData{}); // Empty placeholder
        }
    }

  // 2. Load Materials
  if (model.materials.empty ())
    {
      sceneData->materials.push_back (MaterialData ()); // Default material
    }

  for (const auto &mat : model.materials)
    {
      MaterialData mData;
      std::vector<double> baseColor = mat.pbrMetallicRoughness.baseColorFactor;
      mData.baseColorFactor
          = glm::vec4 (baseColor[0], baseColor[1], baseColor[2], baseColor[3]);
      mData.metallicFactor = (float)mat.pbrMetallicRoughness.metallicFactor;
      mData.roughnessFactor = (float)mat.pbrMetallicRoughness.roughnessFactor;

      mData.baseColorIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
      mData.metallicRoughnessIndex
          = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
      mData.normalIndex = mat.normalTexture.index;

      sceneData->materials.push_back (mData);
    }

  // 3. Load Meshes (Iterate nodes to find meshes)
  const tinygltf::Scene &scene
      = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

  // Simple recursive node traverser
  std::vector<int> nodesToVisit = scene.nodes;
  while (!nodesToVisit.empty ())
    {
      int nodeIdx = nodesToVisit.back ();
      nodesToVisit.pop_back ();
      const tinygltf::Node &node = model.nodes[nodeIdx];

      // Add children
      nodesToVisit.insert (nodesToVisit.end (), node.children.begin (),
                           node.children.end ());

      if (node.mesh > -1)
        {
          const tinygltf::Mesh &mesh = model.meshes[node.mesh];

          for (const auto &primitive : mesh.primitives)
            {
              SubMesh subMesh;

              // Get Material Index
              subMesh.materialIndex = primitive.material;
              if (subMesh.materialIndex < 0)
                subMesh.materialIndex = 0; // fallback to default

              // --- Accessors ---
              const float *bufferPos = nullptr;
              const float *bufferNormals = nullptr;
              const float *bufferTexCoords = nullptr;
              int stridePos = 0, strideNorm = 0, strideTex = 0;
              size_t count = 0;

              // Position
              if (primitive.attributes.find ("POSITION")
                  != primitive.attributes.end ())
                {
                  const tinygltf::Accessor &acc
                      = model.accessors[primitive.attributes.at ("POSITION")];
                  const tinygltf::BufferView &view
                      = model.bufferViews[acc.bufferView];
                  bufferPos = reinterpret_cast<const float *> (
                      &model.buffers[view.buffer]
                           .data[acc.byteOffset + view.byteOffset]);
                  stridePos
                      = acc.ByteStride (view)
                            ? (acc.ByteStride (view) / sizeof (float))
                            : tinygltf::GetNumComponentsInType (acc.type);
                  count = acc.count;
                }

              // Normal
              if (primitive.attributes.find ("NORMAL")
                  != primitive.attributes.end ())
                {
                  const tinygltf::Accessor &acc
                      = model.accessors[primitive.attributes.at ("NORMAL")];
                  const tinygltf::BufferView &view
                      = model.bufferViews[acc.bufferView];
                  bufferNormals = reinterpret_cast<const float *> (
                      &model.buffers[view.buffer]
                           .data[acc.byteOffset + view.byteOffset]);
                  strideNorm
                      = acc.ByteStride (view)
                            ? (acc.ByteStride (view) / sizeof (float))
                            : tinygltf::GetNumComponentsInType (acc.type);
                }

              // TexCoord 0
              if (primitive.attributes.find ("TEXCOORD_0")
                  != primitive.attributes.end ())
                {
                  const tinygltf::Accessor &acc
                      = model
                            .accessors[primitive.attributes.at ("TEXCOORD_0")];
                  const tinygltf::BufferView &view
                      = model.bufferViews[acc.bufferView];
                  bufferTexCoords = reinterpret_cast<const float *> (
                      &model.buffers[view.buffer]
                           .data[acc.byteOffset + view.byteOffset]);
                  strideTex
                      = acc.ByteStride (view)
                            ? (acc.ByteStride (view) / sizeof (float))
                            : tinygltf::GetNumComponentsInType (acc.type);
                }

              // Assemble Vertices
              for (size_t i = 0; i < count; i++)
                {
                  Vertex v;
                  if (bufferPos)
                    {
                      v.position = glm::vec3 (bufferPos[i * stridePos + 0],
                                              bufferPos[i * stridePos + 1],
                                              bufferPos[i * stridePos + 2]);
                    }
                  if (bufferNormals)
                    {
                      v.normal = glm::vec3 (bufferNormals[i * strideNorm + 0],
                                            bufferNormals[i * strideNorm + 1],
                                            bufferNormals[i * strideNorm + 2]);
                    }
                  else
                    {
                      v.normal = glm::vec3 (0, 1, 0);
                    }
                  if (bufferTexCoords)
                    {
                      v.texCoords
                          = glm::vec2 (bufferTexCoords[i * strideTex + 0],
                                       bufferTexCoords[i * strideTex + 1]);
                    }
                  else
                    {
                      v.texCoords = glm::vec2 (0, 0);
                    }

                  // UPDATE BOUNDS
                  if (bufferPos)
                    {
                      globalMin = glm::min (globalMin, v.position);
                      globalMax = glm::max (globalMax, v.position);
                    }

                  subMesh.vertices.push_back (v);
                }

              // Indices
              if (primitive.indices > -1)
                {
                  const tinygltf::Accessor &acc
                      = model.accessors[primitive.indices];
                  const tinygltf::BufferView &view
                      = model.bufferViews[acc.bufferView];
                  const unsigned char *bufferIndex
                      = &model.buffers[view.buffer]
                             .data[acc.byteOffset + view.byteOffset];
                  int strideIndex = acc.ByteStride (view);

                  for (size_t i = 0; i < acc.count; i++)
                    {
                      unsigned int val = 0;
                      if (acc.componentType
                          == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                        {
                          val = *(reinterpret_cast<const unsigned short *> (
                              bufferIndex
                              + i * (strideIndex ? strideIndex : 2)));
                        }
                      else if (acc.componentType
                               == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                        {
                          val = *(reinterpret_cast<const unsigned int *> (
                              bufferIndex
                              + i * (strideIndex ? strideIndex : 4)));
                        }
                      else if (acc.componentType
                               == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                        {
                          val = *(reinterpret_cast<const unsigned char *> (
                              bufferIndex
                              + i * (strideIndex ? strideIndex : 1)));
                        }
                      subMesh.indices.push_back (val);
                    }
                }

              sceneData->meshes.push_back (subMesh);
            }
        }
    }

  sceneData->minBounds = globalMin;
  sceneData->maxBounds = globalMax;

  emit finished (sceneData);
}
