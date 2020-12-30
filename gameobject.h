#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "mesh.h"
#include <cstdint>

class GameObject {
public:
  GameObject();

  Mesh mesh;
  uint32_t textureId{0};
  uint32_t shaderId{0};
};

#endif // GAMEOBJECT_H
