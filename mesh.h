#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 Coord;
  glm::vec3 Normal;
  glm::vec2 TextureCoords;
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indicies;
  std::vector<uint32_t> texture_indicies;
  std::vector<uint32_t> normal_indicies;
  float width{0};
  float height{0};
};
