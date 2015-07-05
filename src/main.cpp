#include "util.h"

ocean_t *ocean = NULL;
camera_t* camera = NULL;

const char* vsrc = ""
"#version 150 core										\n"
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
"#version 150 core 										\n"
"														\n"
"uniform vec2 resolution;								\n"
"														\n"
"out vec4 color;										\n"
"														\n"
"void main(void)										\n"
"{														\n"
"	color = vec4(0.0f, 0.0f, 1.0f, 1.0f);				\n"
"}														\n";

GLuint shader_program;

bool init_simulation(void)
{
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);

 	shader_program = util::create_shader_program(vsrc, fsrc);
	assert(shader_program);

	glUseProgram(shader_program);
	{
		GLint l = glGetUniformLocation(shader_program, "resolution");
		glUniform2f(l, (GLfloat)window_width, (GLfloat)window_height);
	}
	glUseProgram(0);  

	ocean = new ocean_t;

	camera = new camera_t(	glm::vec3(0.0f, 2.0f, 0.0f), 
							45.0f, 
							(float)window_height / (float)window_width,
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

		// update shader uniforms and render ocean
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
}

int main(int argc, char const *argv[])
{
	// at exit callback
	std::atexit(pfn_atexit);

	util::system_setup();

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

	util::system_teardown();

	return EXIT_SUCCESS;
}