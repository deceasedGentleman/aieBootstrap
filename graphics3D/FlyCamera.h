#pragma once
#include "Camera.h"
#include "Input.h"

class FlyCamera :
   public Camera
{
public:
   FlyCamera();
   FlyCamera(float FOV, float AR, glm::vec3 pos, glm::vec3 lookTo, glm::vec3 up);
   ~FlyCamera();

   virtual void update(float deltaTime);
   void setSpeed(float speed);
   void setClamp(float clampAngle);

   void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up = { 0,1,0 });

private:
   glm::vec3 m_up = { 0,1,0 };

   float m_moveSpeed = 8.0f;
   float m_clampAngle = 90.0f;
   float m_MouseSensitivity = 0.3f;

   void MouseLook(float deltaTime, aie::Input * input);
   void WASDMove(float deltaTime, aie::Input * input);
};

