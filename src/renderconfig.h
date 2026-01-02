#ifndef RENDERCONFIG_H
#define RENDERCONFIG_H

struct RenderConfig
{
  bool useBaseColorMap = true;
  bool useMetallicMap = true;
  bool useRoughnessMap = true;
  bool useNormalMap = true;
  bool wireframe = false;
};

#endif // RENDERCONFIG_H
