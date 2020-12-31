#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "mesh.h"
#include <cstdint>

class GameObject {
public:
  GameObject();

  Mesh mesh;
  glm::vec3 movement{0.0f, 0.0f, 0.0f};
  glm::vec3 rotation{0.0f, 0.0f, 0.0f};
  glm::vec3 scale{8.0f, 8.0f, 8.0f};

  uint32_t textureId{0};
  uint32_t shaderId{0};
  uint32_t VAO{0};
  uint32_t VBO{0};
  uint32_t EBO{0};
};

#endif // GAMEOBJECT_H
