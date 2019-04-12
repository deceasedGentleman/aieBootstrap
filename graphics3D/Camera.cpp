#include "Camera.h"

using namespace glm;

Camera::Camera()
{
   setLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
   setPerspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
}

Camera::~Camera()
{}


void Camera::updateProjectionView()
{
   m_projectionView = m_projectionMatrix * m_viewMatrix;
}

void Camera::draw()
{
   updateProjectionView();
   aie::Gizmos::draw(m_projectionView);
}

void Camera::setPerspective(float FOV, float aspectRatio, float near = 0.1f, float far = 1000.f)
{
   m_projectionMatrix = perspective(FOV,
                           aspectRatio,
                           near, far);
}


void Camera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up = { 0,1,0 })
{
   m_viewMatrix = lookAt(from, to, up);
}

void Camera::setPosition(glm::vec3 position)
{
   m_position = position;
}

glm::mat4 Camera::getProjectionView()
{
   return m_projectionView;
}
