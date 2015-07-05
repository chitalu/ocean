#ifndef __OCEAN_H__
#define __OCEAN_H__

struct ocean_t
{
	bool setup(void);

	void teardown(void);

	void process_input(int key, int scancode, int action, int mods);

	void update(float dt);

	void render(void);
};

// initial definition in main.cpp
extern ocean_t *ocean;

#endif