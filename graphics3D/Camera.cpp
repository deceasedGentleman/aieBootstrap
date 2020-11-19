#include "Camera.h"
//#include <glm/ext.hpp>
//#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat4;

Camera::Camera()
{
    m_worldTransform = glm::mat4(1);
    setPerspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
   //setLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
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
   m_worldTransform[3] = glm::vec4(position, m_worldTransform[3].w);
   updateView();
}

void Camera::setWorldTransform(glm::mat4 WT)
{
   m_worldTransform = WT;
   updateView();
}

glm::vec3 Camera::getPosition()
{
   return glm::vec3(getWorldTransform()[3]);
}

/// giving up on the maths for the time, going to hard code the rotation in
//glm::vec3 Camera::getRotation()
//{
//    auto R = getWorldTransform();
//    if (R[0][2] < 1)
//    {
//        if (R[0][2] > -1)
//        {
//            return vec3(
//                atan(R[2][1], R[2][2]),
//                asin(-R[0][2]),
//                atan(-R[0][1], R[0][0])
//            );
//        }
//        else
//        {
//            return vec3(
//                -atan(R[2][1], R[2][2]),
//                -glm::half_pi(),
//                atan(-R[0][1], R[0][0])
//            );
//        }
//    }
//    
//}

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
