#include "camera.h"

camera::camera(glm::vec3 pos, float fov, float aspect,
               float near, float far)
    : proj(glm::perspective(fov, aspect, near, far)), matrix(1.0),
      pos(pos), dir(0.0f), right(0.0f), target(0.0f),
      m_horizontal_angle(3.14f), m_vertical_angle(0.0f),
      move_speed(0.0f), mouse_speed(0.1f) 
      { }

camera::~camera(void) 
{ ; }

void camera::setup(void) 
{ ; }

void camera::apply(float dt) 
{
  if (m_showreel)
    pivot(dt);
  else 
  {
    move(dt);
    free_roam();
  }
}

void camera::pivot(float dt) 
{
  float radius = 1.2f, height = 0.8f;
  static float t = 0;
  t = g_anim_timer * 0.3f;

  height = 0.60f + (0.1f * sin(two_pi + g_anim_timer));
  /*m_target.y = 0.2f + (sin(two_pi + g_anim_timer));
  m_target.y *= 0.2f;*/

  pos = glm::vec3(radius * cos(two_pi + t), 
                  height, 
                  radius * sin(two_pi + t));

  m_matrix = glm::lookAt(pos, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

void camera::free_roam(float dt) 
{
  // static std::list<glm::vec2> mouse_pos_history;
  //// Add the curr
  ent mouse position - starting position
  // mouse_pos_history.push_front((glm::vec2)m_mouse_pos);

  // if (mouse_pos_history.size() > 30) {
  //	// Make sure only the last 30 positions are stored
  //	mouse_pos_history.pop_back();
  //}

  // float weight = 1.0f;

  ////Calculate a weighted average
  // for (auto i = std::begin(mouse_pos_history); i !=
  // std::end(mouse_pos_history); ++i)
  //{
  //	m_mouse_pos += (*i) * weight * dt;
  //	weight *= 0.8f;
  //}

  // m_mouse_pos /= 30.0f;

  // Compute new orientation

  glm::vec2 screen_center(g_screen_width / 2, g_screen_height / 2);

  m_horizontal_angle += mouse_speed * dt * float(screen_center.x - m_mouse_pos.x);
  m_vertical_angle += mouse_speed * dt * float(screen_center.y - m_mouse_pos.y);

  // compute a vector that represents, in World Space, the direction in which
  // we’re looking. spherical coordinates to Cartesian coordinates conversion
  this->dir = glm::vec3(  cos(m_vertical_angle) * sin(m_horizontal_angle),
                          sin(m_vertical_angle),
                          cos(m_vertical_angle) * cos(m_horizontal_angle));

  // Right vector
  this->right = glm::vec3(sin(m_horizontal_angle - (3.14f / 2.0f)), 0,
                      cos(m_horizontal_angle - (3.14f / 2.0f)));

  // Up vector : perpendicular to both direction and right
  glm::vec3 up = glm::cross(this->right, this->dir);

  this->matrix = glm::lookAt(pos, pos + dir, up);
}

void camera::move(void) 
{
  glm::vec3 frwd_vel(dir * dt * move_speed);
  glm::vec3 strafe_vel(right * dt * move_speed);
  static bool lstmv_frwd = false, lstmv_rt = false;

  if (m_move_forward || m_move_back || m_move_left || m_move_right) 
  {
    move_speed += dt * 0.2f;
    glm::clamp(move_speed, 0.0f, 16.0f);
  } 
  else 
  {
    /*apply momentum...*/
    move_speed *= 0.97f;
    if (!m_strafed)
    {
      pos += (frwd_vel * ((lstmv_frwd) ? 1.0f : -1.0f));
    }

    if (!m_moved_back_or_forth)
    {
      pos += (strafe_vel * ((lstmv_rt) ? 1.0f : -1.0f));
    }
  }

  if (m_move_forward) 
  {
    pos += frwd_vel;
    lstmv_frwd = true;
  } 
  else if (m_move_back) 
  {
    pos -= frwd_vel;
    lstmv_frwd = false;
  }

  if (m_move_right) 
  {
    pos += strafe_vel;
    lstmv_rt = true;
  } 
  else if (m_move_left) 
  {
    pos -= strafe_vel;
    lstmv_rt = false;
  }
}

void camera::process_input( GLFWwindow* window, int key, int scancode, 
                            int action, int mods) 
{
  if (input_handler.type == SDL_MOUSEMOTION) 
  {
    glm::ivec2 p;
    SDL_GetMouseState(&p.x, &p.y);
    m_mouse_pos = (glm::vec2)(p);
  }

  if (input_handler.type == SDL_KEYDOWN) {
    switch (input_handler.key.keysym.sym) {
    case SDLK_w:
      m_move_forward = true;
      break;
    case SDLK_s:
      m_move_back = true;
      break;
    case SDLK_d:
      m_move_right = true;
      break;
    case SDLK_a:
      m_move_left = true;
      break;
    case SDLK_v:
      m_showreel = m_showreel ? false : true;
      break;
    }

  } else if (input_handler.type == SDL_KEYUP) {
    switch (input_handler.key.keysym.sym) {
    case SDLK_w:
      m_move_forward = false;
      break;
    case SDLK_s:
      m_move_back = false;
      break;
    case SDLK_d:
      m_move_right = false;
      break;
    case SDLK_a:
      m_move_left = false;
      break;
    }

    switch (input_handler.key.keysym.sym) {
    case SDLK_w:
    case SDLK_s:
      m_moved_back_or_forth = true;
      m_strafed = false;
      break;
    case SDLK_d:
    case SDLK_a:
      m_strafed = true;
      m_moved_back_or_forth = false;
      break;
    }
  }
}