#include "Camera.h"

using namespace glm;

Camera::Camera()
{
   setLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
   setPerspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);
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

void Camera::setPerspective(float FOV, float aspectRatio,float near = 0.1f, float far = 1000.f)
{
   m_projectionMatrix = perspective(FOV,
                           aspectRatio,
                           near, far);
}

void Camera::updateWorldTransform()
{
   m_worldTransform = inverse(m_viewMatrix);
}

void Camera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
   m_viewMatrix = lookAt(from, to, up);
   updateWorldTransform();
}

void Camera::setPosition(glm::vec3 position)
{
   m_worldTransform = translate(mat4(0), position);
   m_viewMatrix = inverse(m_worldTransform);
}

glm::mat4 Camera::getProjectionView()
{
   return m_projectionView;
}
