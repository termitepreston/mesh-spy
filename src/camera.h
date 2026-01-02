#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
  Camera ();

  // Setup
  void setViewportSize (int width, int height);
  void setTarget (const glm::vec3 &target);
  void setDistance (float distance);

  // Input Processing
  void rotate (float dTheta, float dPhi); // Orbit
  void pan (float dx, float dy);
  void zoom (float dDistance);

  // Matrices
  glm::mat4 getViewMatrix () const;
  glm::mat4 getProjectionMatrix () const;
  glm::vec3 getPosition () const;

private:
  void updateVectors ();

  int m_width;
  int m_height;

  // Spherical Coordinates
  float m_theta; // Azimuth (horizontal)
  float m_phi;   // Polar (vertical)
  float m_radius;

  glm::vec3 m_target;
  glm::vec3 m_position;
  glm::vec3 m_up;
  glm::vec3 m_right;
  glm::vec3 m_forward;
};

#endif // CAMERA_H
