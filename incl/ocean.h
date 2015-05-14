#ifndef __OCEAN_H__
#define __OCEAN_H__

struct ocean_t
{
	bool setup(void);

	bool teardown(void);

	void process_input(	GLFWwindow* window, int key, int scancode, 
						int action, int mods);

	void update(float dt);

	void render(void);
};

extern ocean_t *ocean;

#endif