#pragma once
#include "Camera.h"
class FlyCamera :
   public Camera
{
public:
   FlyCamera();
   ~FlyCamera();

   virtual void update(float deltaTime);
   void setSpeed(float speed);
   void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);

private:
   glm::vec3 m_up;

   float m_moveSpeed = 4.0f;
   float m_clampAngle = 90.0f;
   float m_mouseSensitivity = 100.0f;
};

