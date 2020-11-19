#pragma once
#include "Camera.h"
#include "Input.h"

class FlyCamera :
   public Camera
{
public:
   FlyCamera();
   FlyCamera(float FOV, float AR, glm::vec3 pos, float XAngle, float YAngle);
   ~FlyCamera();

   void update(float deltaTime, aie::Input* input);

   virtual void update(float deltaTime);
   void setSpeed(float speed);
   void setClamp(float clampAngle);
   glm::vec2 getRotation();
protected:
   glm::vec3 m_up = { 0,1,0 };

   float m_moveSpeed = 8.0f;
   float m_clampAngle = 90.0f;
   float m_MouseSensitivity = 0.3f;
public:
   float m_XAngle = 0, m_YAngle = 0;
};

