#include "base.h"

#include "ocean.h"
#include "camera.h"

GLFWwindow* window = NULL;
int window_width = (768);
int window_height = (480);
double cursor_posx(0.0);
double cursor_posy(0.0);
bool executing = false;

ocean_t *ocean = NULL;
camera_t* camera = NULL;

const char* vsrc = ""
"#version 150											\n"
"														\n"
"uniform mat4 mvp;										\n"
"uniform mat3 normal_matrix;							\n"
"uniform vec3 view_pos;									\n"
"														\n"
"in vec3 a_pos;											\n"
"														\n"
"void main(void)										\n"
"{														\n"
"	gl_Position = (mvp * vec4(a_pos, 1.0f));			\n"
"}														\n";

// fragment shader
const char* fsrc = ""
"#version 150											\n"
"														\n"
"uniform vec2 resolution;								\n"
"														\n"
"out vec4 color;										\n"
"														\n"
"void main(void)										\n"
"{														\n"
"	color = vec4(0.0f, 1.0f, 1.0f, 1.0f);				\n"
"}														\n";

GLuint shader_program;

// Creates a shader object of the specified type using the specified text
static GLuint create_shader(GLenum type, const char* text)
{
	GLuint shader;
	GLint shader_ok;
	GLsizei log_length;
	char info_log[8192];

	shader = glCreateShader(type);
	if (shader != 0)
	{
		glShaderSource(shader, 1, (const GLchar**)&text, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
		if (shader_ok != GL_TRUE)
		{
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
static GLuint create_shader_program(const char* vs_text, const char* fs_text)
{
	GLuint program = 0u;
	GLint program_ok;
	GLuint vertex_shader = 0u;
	GLuint fragment_shader = 0u;
	GLsizei log_length;
	char info_log[8192];

	vertex_shader = create_shader(GL_VERTEX_SHADER, vs_text);
	if (vertex_shader != 0u)
	{
		fragment_shader = create_shader(GL_FRAGMENT_SHADER, fs_text);
		if (fragment_shader != 0u)
		{
			// make the program that connect the two shader and link it
			program = glCreateProgram();
			if (program != 0u)
			{
				// attach both shader and link
				glAttachShader(program, vertex_shader);
				glAttachShader(program, fragment_shader);
				glLinkProgram(program);
				glGetProgramiv(program, GL_LINK_STATUS, &program_ok);

				if (program_ok != GL_TRUE)
				{
					fprintf(stderr, "ERROR: failed to link shader program\n");
					glGetProgramInfoLog(program, 8192, &log_length, info_log);
					fprintf(stderr, "ERROR: \n%s\n\n", info_log);
					glDeleteProgram(program);
					glDeleteShader(fragment_shader);
					glDeleteShader(vertex_shader);
					program = 0u;
				}
			}
		}
		else
		{
			fprintf(stderr, "ERROR: Unable to load fragment shader\n");
			glDeleteShader(vertex_shader);
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Unable to load vertex shader\n");
	}
	return program;
}

bool init_simulation(void)
{
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

 	shader_program = create_shader_program(vsrc, fsrc);

	glUseProgram(shader_program);
	{
		GLint l = glGetUniformLocation(shader_program, "resolution");
		glUniform2f(l, (GLfloat)window_width, (GLfloat)window_height);
	}
	glUseProgram(0);  

	ocean = new ocean_t;

	camera = new camera_t(	glm::vec3(0.0f, 0.0f, 0.0f), 
							45.0f, 
							float(window_height / window_width), 
							1.0f, 
							1000.0f);

	if (!(executing = camera->setup()))
	{
		return false;
	}

	if (!(executing = ocean->setup()))
	{
		return false;
	}

	return executing;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	cursor_posx = xpos;
	cursor_posy = ypos;
}

// key/input event register callback
static void key_callback(	GLFWwindow* window, int key, int scancode, 
							int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        executing = false;
        return;
    }

	camera->process_input(key, scancode, action, mods);
}

void run_simulation(void)
{
	struct{
		double current, previous, delta;
	}time;

	time.previous = glfwGetTime();
	
	while (executing)
	{
		time.current = glfwGetTime();

	    // determine time difference in milliseconds
	    time.delta = (time.current - time.previous)/1000.0;

		// update camera orientation based on user input
		camera->apply(static_cast<float>(time.delta));

	    // update simulation
	    ocean->update(static_cast<float>(time.delta));

		// clear respective buffers first before rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		{
			glm::mat4	model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0)), 
						view = camera->get_matrix(),
						proj = camera->get_proj(),
						mvp;
			
 			mvp = proj * view * model;

			GLint loc = glGetUniformLocation(shader_program, "mvp");
			glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mvp));

			glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
			loc = glGetUniformLocation(shader_program, "normal_matrix");
			glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(normal_matrix));

			loc = glGetUniformLocation(shader_program, "view_pos");
 			glUniform3fv(loc, 1, glm::value_ptr(camera->get_pos()));

			// render simulation results
			ocean->render();
		}
		glUseProgram(0);

	    // swap front and back buffers
	    glfwSwapBuffers(window);

	    // first processe only those events that have already been 
	    // received and then returns immediately (polling)
	    glfwPollEvents();

	    time.previous = time.current;
	}
}

// In case a GLFW function fails, an error is reported to the 
// GLFW error callback
void glfw_err_callback(int error, const char* description)
{
    fputs(description, stderr);
}

void pfn_atexit(void)
{
	if (camera != NULL)
	{
		camera->teardown();

		delete camera;
		camera = NULL;
	}

	if(ocean != NULL)
	{
		ocean->teardown();
		
		delete ocean;
		ocean = NULL;
	}

	glDeleteProgram(shader_program);

	// When a window is no longer needed, destroy it.
	if (window != NULL)
	{
		glfwDestroyWindow(window);
	}

	// destroys any remaining windows and releases any other 
	// resources allocated by GLFW
	glfwTerminate();
}

int main(int argc, char const *argv[])
{
	// at exit callback
	std::atexit(pfn_atexit);

	// Callback functions must be set, so GLFW knows to call them. 
	// The function to set the error callback is one of the few 
	// GLFW functions that may be called before initialization, 
	// which lets you be notified of errors both during and after 
	// initialization.
	glfwSetErrorCallback(glfw_err_callback);

	GLboolean glfw_status = glfwInit();

	if (glfw_status == GL_FALSE)
	{
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

	if(window == NULL)
	{
		std::exit(EXIT_FAILURE);
	}

	// make current OpenGL context
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// how often we swap render buffers i.e. front and back buffers upon
	// invoking a swap-buffers command
	glfwSwapInterval(1);

	// The key callback is set per-window.
	glfwSetKeyCallback(window, key_callback);

	// set a cursor position callback for "window".
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// retrieves the size, in pixels, of the framebuffer of 
	// the specified window.
	glfwGetFramebufferSize(window, &window_width, &window_height);

	// GLFW_CURSOR_DISABLED hides and grabs the cursor, providing virtual and 
	// unlimited cursor movement. This is useful for implementing for example 
	// 3D camera controls.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// initiliaze OpenGL buffers and shaders etc. necessary to both update
	// and render the ocean waves
	bool status = init_simulation();

	if (!status)
	{
		// we failed!
		fprintf(stderr, "failed to initialize simulation\n");
		std::exit(EXIT_FAILURE);
	}
	else
	{
		// witness the magic...
		run_simulation();
	}

	return EXIT_SUCCESS;
}