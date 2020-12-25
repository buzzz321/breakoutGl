#include "wavefrontreader.h"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

WaveFrontReader::WaveFrontReader(std::string filename) : m_filename(filename) {}

void WaveFrontReader::readVertices(Mesh &obj) {
  std::ifstream myfile(m_filename);
  std::string line;
  uint32_t vertexIndex{0};
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> textureCoords;
  std::vector<glm::vec3> normals;
  std::unordered_map<uint64_t, uint32_t> faces;

  if (myfile.is_open()) {
    while (getline(myfile, line)) {
      auto vpos = line.find("v ");
      if (vpos == 0) {
        std::stringstream conv{line.substr(1)};
        float x, y, z;
        conv >> x >> y >> z;
        std::cout << "v x " << x << " y " << y << " z " << z << std::endl;
        vertices.push_back(glm::vec3(x, y, z));
        continue;
      }
      vpos = line.find("vt ");
      if (vpos == 0) {
        std::stringstream conv{line.substr(2)};
        float x, y;
        conv >> x >> y;
        std::cout << "vt x " << x << " y " << y << std::endl;
        textureCoords.push_back(glm::vec2(x, -y));
        continue;
      }
      vpos = line.find("vn ");
      if (vpos == 0) {

        std::stringstream conv{line.substr(2)};
        float x, y, z;
        conv >> x >> y >> z;
        std::cout << "vn x " << x << " y " << y << std::endl;
        normals.push_back(glm::vec3(x, y, z));

        continue;
      }
      vpos = line.find("f ");
      if (vpos == 0) {
        // f 1/2/3 1/2/3 13/1/2
        // f 11/108/34 13/109/34 7/106/34
        std::istringstream splitter{line.substr(2)};
        std::cout << line << std::endl;
        std::string face;

        Vertex vert;
        while (std::getline(splitter, face, ' ')) {
          uint64_t indexV{0}, indexVt{0}, indexVn{0};

          std::cout << face << std::endl;
          auto start_pos = face.find('/'); // get index for vertex
          indexV = std::stoul(face.substr(0, start_pos), nullptr);
          vert.Coord = vertices[indexV - 1];
          auto end_pos = face.find('/', start_pos + 1); // get index for texture
          if (start_pos != end_pos - 1) {
            indexVt = std::stoul(
                face.substr(start_pos + 1, end_pos - (start_pos + 1)), nullptr);
            obj.texture_indicies.push_back((uint32_t)indexVt - 1);
            vert.TextureCoords = textureCoords[indexVt - 1];
          }
          if (start_pos != end_pos - 1) { // get index for normal
            indexVn = std::stoul(face.substr(end_pos + 1), nullptr);
            obj.normal_indicies.push_back((uint32_t)indexVn - 1);
            vert.Normal = normals[indexVn - 1];
          }
          uint64_t faceKey = indexV | indexVt << 16;
          auto foundPos = faces.find(faceKey);
          if (foundPos != faces.end()) {
            obj.indicies.push_back(faces[faceKey]);
          } else {
            faces[faceKey] = vertexIndex;
            obj.vertices.push_back(vert);
            obj.indicies.push_back(vertexIndex);
            vertexIndex++;
          }
        }
      }
    }
    for (const auto &vert : obj.vertices) {
      std::cout << "v x " << vert.Coord.x << " y " << vert.Coord.y << " z "
                << vert.Coord.z << std::endl;
    }
    for (const auto &index : obj.indicies) {
      std::cout << "index " << index << std::endl;
    }

    obj.indicies.shrink_to_fit();
    obj.vertices.shrink_to_fit();
    myfile.close();
  } else
    std::cout << "Unable to open file";
}
