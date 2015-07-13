#include "util.h"

GLFWwindow *window = NULL;
int window_width = (768);
int window_height = (480);
double cursor_posx(0);
double cursor_posy(0);
bool executing = false;

// In case a GLFW function fails, an error is reported to the
// GLFW error callback
static void pfn_glfw_err_cb(int error, const char *description) {
  fputs(description, stderr);
}

static void pfn_glfw_curspos_cb(GLFWwindow *window, double xpos, double ypos) {
  cursor_posx = xpos;
  cursor_posy = ypos;
}

// key/input event register callback
static void pfn_glfw_key_cb(GLFWwindow *window, int key, int scancode,
                            int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    executing = false;
    return;
  }

  camera->process_input(key, scancode, action, mods);
  ocean->process_input(key, scancode, action, mods);
}

void util::system_setup(void) {
  // Callback functions must be set, so GLFW knows to call them.
  // The function to set the error callback is one of the few
  // GLFW functions that may be called before initialization,
  // which lets you be notified of errors both during and after
  // initialization.
  glfwSetErrorCallback(pfn_glfw_err_cb);

  GLboolean glfw_status = glfwInit();

  if (glfw_status == GL_FALSE) {
    std::exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  // The window and its OpenGL context are created with a single
  // call, which returns a handle to the created combined window
  // and context object.
  window = glfwCreateWindow(window_width, window_height, "Ocean", NULL, NULL);

  if (window == NULL) {
    std::exit(EXIT_FAILURE);
  }

  // make current OpenGL context
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // how often we swap render buffers i.e. front and back buffers upon
  // invoking a swap-buffers command
  glfwSwapInterval(1);

  // The key callback is set per-window.
  glfwSetKeyCallback(window, pfn_glfw_key_cb);

  // set a cursor position callback for "window".
  // glfwSetCursorPosCallback(window, pfn_glfw_curspos_cb);

  // retrieves the size, in pixels, of the framebuffer of
  // the specified window.
  glfwGetFramebufferSize(window, &window_width, &window_height);

  // GLFW_CURSOR_DISABLED hides and grabs the cursor, providing virtual and
  // unlimited cursor movement. This is useful for implementing for example
  // 3D camera controls.
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void util::system_teardown(void) {
  // When a window is no longer needed, destroy it.
  if (window != NULL) {
    glfwDestroyWindow(window);
  }

  // destroys any remaining windows and releases any other
  // resources allocated by GLFW
  glfwTerminate();
}

std::string util::load_src(const char *path) {
  printf("load shader: %s\n", path);
  // ...

  // printf("size: %d bytes");

  return std::string();
}

GLuint create_shader(GLenum type, const char *path) {
  GLuint shader;
  GLint shader_ok;
  GLsizei log_length;
  char info_log[8192];

  shader = glCreateShader(type);
  if (shader != 0) {
    glShaderSource(shader, 1, (const GLchar **)&path, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);

    if (shader_ok != GL_TRUE) {
      fprintf(stderr, "ERROR: Failed to compile %s shader\n",
              (type == GL_FRAGMENT_SHADER) ? "fragment" : "vertex");

      glGetShaderInfoLog(shader, 8192, &log_length, info_log);
      fprintf(stderr, "ERROR: \n%s\n\n", info_log);
      glDeleteShader(shader);
      shader = 0;
    }
  }
  return shader;
}

// Creates a program object using the specified vertex and fragment text
GLuint util::create_shader_program(const char *vs_text, const char *fs_text) {
  GLuint program = 0u;
  GLint program_ok;
  GLuint vertex_shader = 0u;
  GLuint fragment_shader = 0u;
  GLsizei log_length;
  char info_log[8192];

  vertex_shader = create_shader(GL_VERTEX_SHADER, vs_text);

  if (vertex_shader != 0u) {
    fragment_shader = create_shader(GL_FRAGMENT_SHADER, fs_text);
    if (fragment_shader != 0u) {
      // make the program that connect the two shader and link it
      program = glCreateProgram();
      if (program != 0u) {
        // attach both shader and link
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &program_ok);

        if (program_ok != GL_TRUE) {
          fprintf(stderr, "ERROR: failed to link shader program\n");
          glGetProgramInfoLog(program, 8192, &log_length, info_log);
          fprintf(stderr, "ERROR: \n%s\n\n", info_log);
          glDeleteProgram(program);
          glDeleteShader(fragment_shader);
          glDeleteShader(vertex_shader);
          program = 0u;
        }
      }
    } else {
      fprintf(stderr, "ERROR: Unable to load fragment shader\n");
      glDeleteShader(vertex_shader);
    }
  } else {
    fprintf(stderr, "ERROR: Unable to load vertex shader\n");
  }
  return program;
}
