#pragma once
#include <glew\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <map>
#include <string>
#include <vector>

class Shader;

void init();
void initSizing();
void update();
void render();
void cleanup();
int main();
void key(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* window;

double lastTime;
float dt;

GLuint vao;
GLuint vbo;
GLuint ibo;

bool useInstanceRendering = true;

Shader* currentShader;
Shader* defaultShader;
Shader* instanceShader;
glm::mat4 proj;
glm::mat4 view;
glm::mat4 translation;
glm::mat4 rotation;
glm::mat4 scale;
glm::mat4* model = nullptr;

float shiftSpeed = 0.25f;
float hue = 0;
glm::vec3 hsv;
glm::vec3 rgb;

int numCubes = 3;
int totalCubes;

float rot = 0;
float rotateSpeed = 60.0f;

float camRot = 0;
float camRotateSpeed = 10.0f;