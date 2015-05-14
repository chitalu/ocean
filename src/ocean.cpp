#include "obj.h"

#include "base.h"

#include "camera.h"

#define vATTRIB_POS (0)

struct 
{
	GLint 	vao, // vertex array object 
			vbo, // vertex buffer object
			ibo, // index buffer object
			icount, // number of indices 
			length, // length along x 
			breadth; // length along z
}obj;

bool ocean_t::setup(void)
{
	obj.length = 256;
	obj.breadth = 256;

	// must be even
	assert((obj.length & obj.breadth) & 2);

	// define vertex array object
	glGenVertexArrays(1, &obj.vao);
	glBindVertexArray(obj.vao);
	{
		// define vertex buffer object for vertex positions
		glGenBuffers(1, &obj.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
		{
			// allocate buffer memory
			glBufferData();
			// specify buffer data interpretation upon rendering
			glVertexAttribPointer();

			glm::vec3* gpu_vptr = (glm::vec3*)glMapBuffer();
			assert(gpu_vptr != NULL && "failed to map vertex buffer");

			const float hlength = obj.length / 2.0f;
			const float hbreadth = obj.breadth / 2.0f;

			// fill out vertex data representing grid
			for (int x(0); x < obj.length; x++) 
			{
				for (int z(0); z < obj.breadth; z++) 
				{
					gpu_vptr[x + (z * obj.length)] = glm::vec3(	x - hlength, 
																0.0f, 
																z - hbreadth);
				}
			}

			GLboolean result = glUnMapBuffer();
			assert(result == GL_TRUE && "failed to unmap vertex buffer");
			gpu_vptr = NULL;
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// define vertex buffer object for vertex indices
		glGenBuffers(1, &obj.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibo);
		{
			glBufferData();

			GLuint* gpu_iptr = (GLuint*)glMapBuffer();
			assert(gpu_iptr != NULL && "failed to map index buffer");

			int cnt = 0, tris_per_sqr = 2, indices_per_sqr = 4;
			// number of indices is equal to:
			obj.icount = 	((obj.length - 1) * (obj.breadth - 1)) *
                           	tris_per_sqr * indices_per_sqr;

			// define the index data ...
			for (int x = 0; x < (obj.length - 1); x++) 
			{
				for (int z = 0; z < (obj.breadth - 1); z++) 
				{
					gpu_iptr[cnt + 0] = (x + (z * obj.length));
					gpu_iptr[cnt + 1] = (x + (z * obj.length) + obj.length);
					gpu_iptr[cnt + 2] = (x + (z * obj.length) + obj.length + 1);

					gpu_iptr[cnt + 3] = (x + (z * obj.length));
					gpu_iptr[cnt + 4] = (x + (z * obj.length) + obj.length + 1);
					gpu_iptr[cnt + 5] = ((x + 1) + (z * obj.length));

					++cnt;
				}
			}

			GLboolean result = glUnMapBuffer();
			assert(result == GL_TRUE && "failed to unmap index buffer");
			gpu_iptr = NULL;
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

bool ocean_t::teardown(void)
{
	glDeleteBuffers(1, &obj.vbo);
	glDeleteBuffers(1, &obj.ibo);

	glDeleteVertexArrays(1, &obj.vao);
}

void ocean_t::process_input(	GLFWwindow* window, int key, int scancode, 
								int action, int mods)
{

}

void ocean_t::update(float dt)
{

}

void ocean_t::render(void)
{
	glBindVertexArray(vertex.array);
	glEnableVertexAttribPointer(vATTRIB_POS);

	glDrawElements(GL_LINES_LOOP, GL_UNSIGNED_INT, obj.icount);

	glDisableVertexAttribPointer(vATTRIB_POS);
	glBindVertexArray(0);
}