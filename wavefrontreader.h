#ifndef WAVEFRONTREADER_H
#define WAVEFRONTREADER_H

#include <glm/glm.hpp>
#include <string>
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
};

class WaveFrontReader {
public:
  WaveFrontReader(std::string filename);

  void readVertices(Mesh &obj);

private:
  std::string m_filename;
};

#endif // WAVEFRONTREADER_H
