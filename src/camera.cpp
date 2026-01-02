#include "camera.h"
#include <algorithm>

Camera::Camera ()
    : m_width (800), m_height (600), m_theta (0.0f),
      m_phi (glm::radians (45.0f)), m_radius (5.0f), m_target (0.0f),
      m_up (0.0f, 1.0f, 0.0f)
{
  updateVectors ();
}

void
Camera::setViewportSize (int width, int height)
{
  m_width = width;
  m_height = height;
}

void
Camera::setTarget (const glm::vec3 &target)
{
  m_target = target;
  updateVectors ();
}

void
Camera::setDistance (float distance)
{
  m_radius = std::max (0.1f, distance); // Prevent 0 or negative
  updateVectors ();
}

void
Camera::rotate (float dTheta, float dPhi)
{
  m_theta += dTheta;
  m_phi -= dPhi; // Subtract to feel natural

  // Clamp phi to prevent flip over
  const float epsilon = 0.001f;
  m_phi = std::clamp (m_phi, epsilon, glm::pi<float> () - epsilon);

  updateVectors ();
}

void
Camera::pan (float dx, float dy)
{
  // Pan speed proportional to distance
  float speed = m_radius * 0.001f;

  glm::vec3 movement = -m_right * dx * speed + m_up * dy * speed;
  m_target += movement;
  updateVectors ();
}

void
Camera::zoom (float amount)
{
  m_radius -= amount;
  if (m_radius < 0.1f)
    m_radius = 0.1f;
  updateVectors ();
}

void
Camera::updateVectors ()
{
  // Spherical to Cartesian
  float x = m_radius * sin (m_phi) * sin (m_theta);
  float y = m_radius * cos (m_phi);
  float z = m_radius * sin (m_phi) * cos (m_theta);

  m_position = m_target + glm::vec3 (x, y, z);

  m_forward = glm::normalize (m_target - m_position);
  m_right
      = glm::normalize (glm::cross (m_forward, glm::vec3 (0.0f, 1.0f, 0.0f)));
  m_up = glm::normalize (glm::cross (m_right, m_forward));
}

glm::mat4
Camera::getViewMatrix () const
{
  return glm::lookAt (m_position, m_target, m_up);
}

glm::mat4
Camera::getProjectionMatrix () const
{
  return glm::perspective (glm::radians (45.0f),
                           (float)m_width / (float)m_height, 0.1f, 100.0f);
}

glm::vec3
Camera::getPosition () const
{
  return m_position;
}
