#include "base.h"

#include "ocean.h"

GLFWwindow* window = NULL;
int window_width = (768);
int window_width = (480);
bool executing = false;

ocean_t *ocean = NULL;

bool init_simulation(void)
{
	glViewport(0, 0, window_width, window_height);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ocean = (ocean_t*)malloc(sizeof(ocean_t));

	if(!(executing = ocean->setup()))
	{
		return false;
	}

	return executing;
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

	ocean->process_input(window, key, scancode, action, mods);
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

	    // update simulation
	    ocean->update(static_cast<float>(time.delta));

	    // render simulation results
		ocean->render();

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
	if(ocean != NULL)
	{
		ocean->teardown();
		
		free(ocean);
		ocean = NULL;
	}

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

	// how often we swap render buffers i.e. front and back buffers upon
	// invoking a swap-buffers command
	glfwSwapInterval(1);

	// The key callback is set per-window.
	glfwSetKeyCallback(window, key_callback);

	// retrieves the size, in pixels, of the framebuffer of 
	// the specified window.
	glfwGetFramebufferSize(window, &window_width, &window_height);

	// initiliaze OpenGL buffers and shaders etc. necessary to both update
	// and render the ocean waves
	bool status = init_simulation();

	if (!status)
	{
		// we failed!
		fprintf(stderr, "failed to initialize simulation\n", );
		std::exit(EXIT_FAILURE);
	}
	else
	{
		// witness the magic...
		run_simulation();
	}

	return EXIT_SUCCESS;
}