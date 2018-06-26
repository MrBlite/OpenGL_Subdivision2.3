#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
//#include "vec3.h"
using namespace std;
using namespace glm;
enum SUB_RESULT_USAGE { DRAW_VERTEX, DRAW_FRAME, DRAW_TRIFACE,DRAW_POLYFACE,DRAW_POLYLINE };