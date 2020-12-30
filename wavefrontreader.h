#ifndef WAVEFRONTREADER_H
#define WAVEFRONTREADER_H

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "mesh.h"

class WaveFrontReader {
public:
  WaveFrontReader(std::string filename);

  void readVertices(Mesh &obj);

private:
  std::string m_filename;
};

#endif // WAVEFRONTREADER_H
