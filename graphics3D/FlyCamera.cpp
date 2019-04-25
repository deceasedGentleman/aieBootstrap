#include "FlyCamera.h"
#include "Input.h"




FlyCamera::FlyCamera()
{}

FlyCamera::FlyCamera(float FOV, float AR, glm::vec3 pos, float VAngle, float HAngle) :
   m_up(0, 1, 0),
   m_XAngle(VAngle),
   m_YAngle(HAngle)
{
   setPerspective(FOV, AR);
   
}

FlyCamera::~FlyCamera()
{}

void FlyCamera::update(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   
   //retrive mouse input
   double mouseDeltaX = input->getMouseDeltaX();
   double mouseDeltaY = input->getMouseDeltaY();

   //invert input if needed
   mouseDeltaX = -mouseDeltaX;

   m_YAngle += (float)mouseDeltaX * m_MouseSensitivity * deltaTime;
   m_XAngle += (float)mouseDeltaY * m_MouseSensitivity * deltaTime;

   glm::clamp(m_XAngle, m_clampAngle, -m_clampAngle);

   // Direction : Spherical coordinates to Cartesian coordinates conversion
   glm::vec3 direction(
      cos(m_XAngle) * sin(m_YAngle),
      sin(m_XAngle),
      cos(m_XAngle) * cos(m_YAngle)
   );

   // Right vector
   glm::vec3 right = glm::vec3(
      sin(m_YAngle - 3.14f / 2.0f),
      0,
      cos(m_YAngle - 3.14f / 2.0f)
   );

   // New Up vector
   glm::vec3 up = glm::cross(right, direction);
   //m_up = up;

   glm::vec3 position = getPosition();


   //horizontal movement
   if (input->isKeyDown(aie::INPUT_KEY_W))
      position += direction * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_A))
      position -= right * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_S))
      position -= direction * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_D))
      position += right * deltaTime * m_moveSpeed;

   //vertical movement 
   if (input->isKeyDown(aie::INPUT_KEY_SPACE))
      position += m_up * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
      position -= m_up * deltaTime * m_moveSpeed;
   // m_up = global up/{0,1,0}
   // use 'up' for relative up

   setLookAt(position, position + direction, up);
}

void FlyCamera::setSpeed(float spd)
{
   m_moveSpeed = spd;
}

void FlyCamera::setClamp(float clampAngle)
{
   m_clampAngle = clampAngle; 
}

