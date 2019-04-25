#include "Camera.h"

using namespace glm;

Camera::Camera()
{
   setLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
   setPerspective(radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
}

Camera::Camera(float FOV, float AR, glm::vec3 pos, glm::vec3 lookTo, glm::vec3 up)
{
   setPerspective(FOV, AR);
   setLookAt(pos, lookTo, up);
}

Camera::~Camera()
{}



void Camera::draw()
{
   aie::Gizmos::draw(getProjectionView());
}

void Camera::setPerspective(float FOV, float aspectRatio, float near, float far)
{
   m_projectionMatrix = glm::perspective(FOV,
                           aspectRatio,
                           near, far);
}

mat4 Camera::getWorldTransform()
{
   return m_worldTransform;
}

void Camera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
   m_viewMatrix = lookAt(from, to, up);
   updateWorld();
}

void Camera::setPosition(glm::vec3 position)
{
   m_worldTransform[3] = vec4(position, m_worldTransform[3].w);
   updateView();
}

void Camera::setWorldTransform(glm::mat4 WT)
{
   m_worldTransform = WT;
   updateView();
}

glm::vec3 Camera::getPosition()
{
   return vec3(getWorldTransform()[3]);
}

glm::vec3 Camera::getRow(unsigned int row)
{
   return glm::vec3(getWorldTransform()[row]);
}

glm::mat4 Camera::getProjectionView()
{
   return m_projectionMatrix * m_viewMatrix;
}

void Camera::updateWorld()
{
   m_worldTransform = glm::inverse(m_viewMatrix);
}

void Camera::updateView()
{
   m_viewMatrix = glm::inverse(m_worldTransform);
}
