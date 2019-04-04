#include "Camera.h"
#include "Gizmos.h"

using namespace glm;

Camera::Camera()
{
   m_viewTransform = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
   m_projectionTransform = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
}

Camera::~Camera()
{}


void Camera::updateProjectionView()
{}

void Camera::draw()
{
   aie::Gizmos::draw(m_projectionView);
}

void Camera::setPerspective(float fieldOfView, float aspectRatio, float, float)
{

}

void Camera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
   m_worldTransform = lookAt(from, to, up);
}

void Camera::setPosition(glm::vec3 position)
{
   m_worldTransform = translate(m_worldTransform, position);
}
