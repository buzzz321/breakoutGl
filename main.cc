#include "glad.h" // must be before glfw.h
#include "wavefrontreader.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "gameobject.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr int32_t SCREEN_WIDTH = 1600;
constexpr int32_t SCREEN_HEIGHT = 1100;
constexpr float fov = glm::radians(90.0f);

constexpr auto vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

constexpr auto fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
  FragColor = texture(texture_diffuse1, TexCoords);
}
)";

unsigned int loadShaders(const char *shaderSource, GLenum shaderType) {

  unsigned int shader{0};
  int success{0};
  char infoLog[1024];

  shader = glCreateShader(shaderType); // GL_VERTEX_SHADER

  glShaderSource(shader, 1, &shaderSource, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  return shader;
}

unsigned int makeShaderProgram(uint32_t vertexShader, uint32_t fragmentShader) {
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

unsigned int loadImage(std::string filename) {
  int width, height, nrChannels;

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  unsigned char *data =
      stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "cant find texture name " << filename << std::endl;
  }
  stbi_image_free(data);

  return texture;
}

void error_callback(int error, const char *description) {
  std::cerr << "Error: " << description << " error number " << error
            << std::endl;
}

void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action,
                  int /*mods*/) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void camera(uint32_t shaderId) {
  glm::mat4 view = glm::mat4(1.0f);

  float zFar = (SCREEN_WIDTH / 2.0f) / tanf(fov / 2.0f); // was 90.0f
  glm::vec3 cameraPos =
      glm::vec3(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f, zFar);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  //  std::cout << " x= " << cameraPos.x << " y = " << cameraPos.y
  //            << " z = " << cameraPos.z << " zFar = " << zFar << " tan µ "
  //            << tanf64(fov / 2.0f) << " fov " << fov << std::endl;
  // glm::vec3 cameraFront = glm::vec3(32.0f, 32.0f, -1.0f);

  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

  int modelView = glGetUniformLocation(shaderId, "view");
  glUniformMatrix4fv(modelView, 1, GL_FALSE, glm::value_ptr(view));
}

int main() {

  float deltaTime = 0.0f; // Time between current frame and last frame
  float lastFrame = 0.0f; // Time of last frame

  srand((unsigned int)time(NULL));

  if (!glfwInit()) {
    // Initialization failed
    std::cerr << "Error could not init glfw!" << std::endl;
    exit(1);
  }

  stbi_set_flip_vertically_on_load(true);

  glfwSetErrorCallback(error_callback);

    glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "My Title",
                                        nullptr, nullptr);
  if (!window) {
    std::cerr << "Error could not create window" << std::endl;
    exit(1);
    // Window or OpenGL context creation failed
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << " Error could not load glad " << std::endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(1);
  }

   glEnable(GL_DEBUG_OUTPUT);
   glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
           GLsizei length, const GLchar* message, const void* userParam) {
            std::cout << "OpenGL Error/Warning: " << message << std::endl;
        },
        nullptr);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Cull triangles which normal is not towards the camera
  (GL_CULL_FACE);

  GameObject pad;

  WaveFrontReader reader("../pad.obj");
  // WaveFrontReader reader("../plane.obj");
  // WaveFrontReader reader("../kub.obj");

  reader.readVertices(pad.mesh);
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);

  unsigned int VBO, EBO;
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               pad.mesh.indicies.size() * sizeof(uint32_t),
               &pad.mesh.indicies[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, pad.mesh.vertices.size() * sizeof(Vertex),
               &pad.mesh.vertices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TextureCoords));

  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  auto vertexShader = loadShaders(vertexShaderSource, GL_VERTEX_SHADER);
  auto fragmentShader = loadShaders(fragmentShaderSource, GL_FRAGMENT_SHADER);
  pad.shaderId = makeShaderProgram(vertexShader, fragmentShader);

  float zFar = (SCREEN_WIDTH / 2.0f) / tanf(fov / 2.0f) + 10.0f; // 100.0f
  glm::mat4 projection = glm::perspective(
      fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, zFar);

  pad.textureId = loadImage("../pad.png");
  glActiveTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(pad.textureId, "texture_diffuse1"), 1);

  //  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

    // 2. use our shader program when we want to render an object
    glUseProgram(pad.shaderId);
    // glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

    int modelprj = glGetUniformLocation(pad.shaderId, "projection");
    glUniformMatrix4fv(modelprj, 1, GL_FALSE, glm::value_ptr(projection));

    camera(pad.shaderId);

    // and finally bind the texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 1);
    glBindVertexArray(VAO);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(800.0f, 400.0f, 200.0f));
    model =
        glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(8.0, 8.0, 8.0));

    int modelLoc = glGetUniformLocation(pad.shaderId, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawElements(GL_TRIANGLES, pad.mesh.indicies.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    // Keep running
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
