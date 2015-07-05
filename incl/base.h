#ifndef __BASE_H__
#define __BASE_H__

// Starting with version 3.0, the GLU header glu.h is no 
// longer included by default. If you wish to include it, 
// define GLFW_INCLUDE_GLU before the inclusion of the GLFW header.
// #define GLFW_INCLUDE_GLU

#include <glad/glad.h>

#include <GLFW/glfw3.h>

// glm::vec3
#include <glm/vec3.hpp> 
// glm::vec4
#include <glm/vec4.hpp> 
// glm::mat4
#include <glm/mat4x4.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp> 
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <cstdint>

#include <cmath>

#include <string>

#include "ocean.h"
#include "camera.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define glchk_ assert(glGetError() == GL_NO_ERROR);

// window handle as returned by glfw
extern GLFWwindow* window;
extern int window_height;
extern int window_width;

extern double cursor_posx;
extern double cursor_posy;

// boolean indicator to signify whether the application 
// is running
extern bool executing;
extern GLuint shader_program;

#endif