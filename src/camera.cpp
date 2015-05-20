#include "camera.h"

camera_t::camera_t(	glm::vec3 pos, float fov, float aspect,
					float z_near, float z_far)
	:	proj(glm::perspective(fov, aspect, z_near, z_far)), 
		matrix(1.0),
		pos(pos), 
		dir(0.0f), 
		right(0.0f),	
		target(glm::vec3(0.0f, 0.0f, 0.0f)),
		horizontal_ang((float)M_PI), 
		vertical_ang(0.0f),
		speed(0.0f), 
		mouse_speed(1.0f),
		showreel(false)
      { }

camera_t::~camera_t(void) 
{ ; }

bool camera_t::setup(void) 
{ 
	for (int i(0); i < INPUT::TOTAL; ++i)
	{
		user[i] = false;
	}

	return true; 
}

void camera_t::teardown(void)
{

}

void camera_t::apply(float dt) 
{
	if (showreel) // pivot around a particular position i.e. "target"
	{
		static float	radius = 10.72f, 
						height = 10.50f,
						t = 0; 
		
		t += dt * 100;

		this->pos = glm::vec3(	radius * cos(float(M_PI * 2.0f) + t),
								height,
								radius * sin(float(M_PI * 2.0f) + t));

		this->matrix = glm::lookAt(	this->pos,
									this->target,
									glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else // move around freely and unrestricted ...
	{
		calc_velocity(dt);
		
		glm::vec2 screen_center(window_width / 2.0f, window_height / 2.0f);

		horizontal_ang += mouse_speed * dt * float(screen_center.x - cursor_posx);
		vertical_ang += mouse_speed * dt * float(screen_center.y - cursor_posy);

		// compute a vector that represents, in world space, the direction in which
		// we’re looking. spherical coordinates to cartesian coordinates conversion
		this->dir = glm::vec3(	cos(vertical_ang) * sin(horizontal_ang),
								sin(vertical_ang),
								cos(vertical_ang) * cos(horizontal_ang));

		// right vector 
		this->right = glm::vec3(sin(horizontal_ang - ((float)(M_PI) / 2.0f)),
								0,
								cos(horizontal_ang - ((float)(M_PI) / 2.0f)));

		// up vector : perpendicular to both direction and right
		glm::vec3 up = glm::cross(this->right, this->dir);

		// form "the" camera matrix
		this->matrix = glm::lookAt(pos, pos + dir, up);
	}
}

void camera_t::calc_velocity(float dt)
{
	glm::vec3	forward_velocity(dir * dt * speed),
				strafing_velocity(right * dt * speed);

	static bool prev_move_foward = false, 
				prev_move_right = false;

	if (user[INPUT::FORWARD] || 
		user[INPUT::BACK] || 
		user[INPUT::LEFT] ||
		user[INPUT::RIGHT])
	{
		speed += dt * 10000.20f;
		speed = glm::clamp(speed, 0.0f, 64.0f);
	} 
	else 
	{
		// apply momentum...
		speed *= 0.97f;

		if (strafing == false)
		{
			pos += (forward_velocity * ((prev_move_foward) ? 1.0f : -1.0f));
		}

		if (moving_back_or_forth == false)
		{
			pos += (strafing_velocity * ((prev_move_right) ? 1.0f : -1.0f));
		}
	}

	if (user[INPUT::FORWARD])
	{
		pos += forward_velocity;
		prev_move_foward = true;
	} 
	else if (user[INPUT::BACK])
	{
		pos -= forward_velocity;
		prev_move_foward = false;
	}

	if (user[INPUT::RIGHT])
	{
		pos += strafing_velocity;
		prev_move_right = true;
	} 
	else if (user[INPUT::LEFT])
	{
		pos -= strafing_velocity;
		prev_move_right = false;
	}
}

void camera_t::process_input( int key, int scancode, int action, int mods) 
{
	if (action == GLFW_PRESS) 
	{
		switch (key) 
		{
		case GLFW_KEY_W:
			user[INPUT::FORWARD] = true;
			break;
		case GLFW_KEY_S:
			user[INPUT::BACK] = true;
			break;
		case GLFW_KEY_D:
			user[INPUT::RIGHT] = true;
			break;
		case GLFW_KEY_A:
			user[INPUT::LEFT] = true;
			break;
		case GLFW_KEY_SPACE:
			showreel = showreel ? false : true;
			break;
		}
	}
	else if (action == GLFW_RELEASE) 
	{
		switch (key) 
		{
		case GLFW_KEY_W:
			user[INPUT::FORWARD] = false;
			break;
		case GLFW_KEY_S:
			user[INPUT::BACK] = false;
			break;
		case GLFW_KEY_D:
			user[INPUT::RIGHT] = false;
			break;
		case GLFW_KEY_A:
			user[INPUT::LEFT] = false;
			break;
		}

		switch (key) 
		{
			case GLFW_KEY_W: case GLFW_KEY_S:
				moving_back_or_forth = true;
				strafing = false;
			break;
			case GLFW_KEY_D: case GLFW_KEY_A:
				strafing = true;
				moving_back_or_forth = false;
			break;
		}	
	}
}