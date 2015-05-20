#include "ocean.h"
#include "camera.h"

#include <cassert>

struct 
{
	GLuint 	vao, // vertex array object 
			vbo, // vertex buffer object
			ibo, // index buffer object
			vpos_attrib_loc;

	GLint	icount, // number of indices 
			length, // length along x 
			breadth; // length along z
}obj;

bool ocean_t::setup(void)
{
	obj.length = 256;
	obj.breadth = 256;

	// must be even
	assert((!(obj.length % 2) && !(obj.breadth % 2)) && "grid dimensions not even");

	// define vertex array object
	glGenVertexArrays(1, &obj.vao);
	glBindVertexArray(obj.vao);
	{
		// define vertex buffer object for vertex positions
		glGenBuffers(1, &obj.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
		{
			// allocate buffer memory
			glBufferData(	GL_ARRAY_BUFFER, 
							sizeof(glm::vec3) * (obj.length * obj.breadth),
							NULL, 
							GL_DYNAMIC_DRAW);
			glchk_;

			// set up vertex arrays
			obj.vpos_attrib_loc = glGetAttribLocation(shader_program, "a_pos");

			// specify buffer data interpretation upon rendering
			glVertexAttribPointer(obj.vpos_attrib_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glchk_;

			glm::vec3* gpu_vptr = (glm::vec3*)glMapBuffer(	GL_ARRAY_BUFFER, 
															GL_WRITE_ONLY);
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

			GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
			assert(result == GL_TRUE && "failed to unmap vertex buffer");
			gpu_vptr = NULL;
		}

		// define vertex buffer object for vertex indices
		glGenBuffers(1, &obj.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ibo);
		{
			int cnt = 0, tris_per_sqr = 2, indices_per_sqr = 4;
			// number of indices is equal to:
			obj.icount =	((obj.length - 1) * (obj.breadth - 1)) *
							tris_per_sqr * indices_per_sqr;

			glBufferData(	GL_ELEMENT_ARRAY_BUFFER, 
							sizeof(GLuint) * obj.icount, 
							NULL, 
							GL_STATIC_DRAW);
			glchk_;

			GLuint* gpu_iptr = (GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, 
													GL_WRITE_ONLY);
			assert(gpu_iptr != NULL && "failed to map index buffer");

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

					cnt += 6;
				}
			}

			GLboolean result = glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			assert(result == GL_TRUE && "failed to unmap index buffer");
			gpu_iptr = NULL;
		}
		
	}
	//glBindVertexArray(0);

	return true;
}

void ocean_t::teardown(void)
{
	glDeleteBuffers(1, &obj.vbo);
	glDeleteBuffers(1, &obj.ibo);

	glDeleteVertexArrays(1, &obj.vao);
}

void ocean_t::process_input(int key, int scancode, int action, int mods)
{

}

void ocean_t::update(float dt)
{

}

void ocean_t::render(void)
{
	glBindVertexArray(obj.vao);
	glEnableVertexAttribArray(obj.vpos_attrib_loc);

	glDrawElements(GL_LINE_LOOP, obj.icount, GL_UNSIGNED_INT, (void*)0);
	glchk_;

	glDisableVertexAttribArray(obj.vpos_attrib_loc);
	glBindVertexArray(0);
}