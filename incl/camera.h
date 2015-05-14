#ifndef __camera_H__
#define __camera_H__

#include "entity.h"
#include "common.h"

struct camera : public Entity 
{
private:
  glm::mat4 proj, matrix;
  glm::vec3 pos;
  glm::vec3 dir;
  glm::vec3 right;
  glm::vec3 target; // look at, as well as pivot around
  // horizontal angle : toward -Z
  float m_horizontal_angle;
  // vertical angle : 0, look at the horizon
  float m_vertical_angle;

  float move_speed; // 3 units / second
  float mouse_speed;
  
  enum INPUT
  {
    FORWARD=0,
    BACK,
    LEFT,
    RIGHT,
    SHOWREEL,
    ROTATE
  };

  bool user[8];

  bool m_showreel, m_move_forward, m_move_back, m_move_left, m_move_right,
      m_moved_back_or_forth, // forward or backward...
      m_strafed, rotating;   // left or right ...

  // makes camera rotate around model
  void pivot(void);

  // user moves around freely
  void free_roam(void);

  void move(void);

public:

  camera(glm::vec3 pos, float fov, float aspect,
         float near_plane, float far_plane);

  ~camera(void);

  void setup(void);

  void apply(float dt);

  // return projection matrix
  inline const glm::mat4 &get_proj(void) const 
  {
    return proj;
  }

  inline virtual const glm::mat4 &get_matrix(void) const 
  { 
    return matrix; 
  }

  // where I'm I looking?
  inline void set_target(glm::vec3 const &t) 
  { 
    target = t; 
  }

  const glm::vec3 &get_pos(void) const 
  {
    return this->pos; 
  }
  
  void rotate(float angle, glm::vec3 axes) 
  {
    m_matrix = glm::translate(glm::mat4(1.0), position);
    m_matrix *= glm::rotate(m_matrix, glm::radians(angle), axes);
  }

  void process_input( GLFWwindow* window, int key, int scancode, 
                      int action, int mods);
};

#endif