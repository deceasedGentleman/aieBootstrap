#include "FlyCamera.h"
#include "Input.h"

using namespace glm;

FlyCamera::FlyCamera()
{}

FlyCamera::~FlyCamera()
{}

void FlyCamera::update(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   glm::vec3 moveVector = vec3(0);
   glm::vec3 deltaMouse = vec3(0);

   //horizontal movement
   if (input->isKeyDown(aie::INPUT_KEY_W))
      moveVector.z -= 1;
   if (input->isKeyDown(aie::INPUT_KEY_S))
      moveVector.z += 1;

   if (input->isKeyDown(aie::INPUT_KEY_A))
      moveVector.x -= 1;
   if (input->isKeyDown(aie::INPUT_KEY_D))
      moveVector.x += 1;

   moveVector *= m_moveSpeed;

   moveVector = 
   //vertical movement 
   if (input->isKeyDown(aie::INPUT_KEY_SPACE))
   if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))

   //rotation
   /*
   int deltaMouseX = input->getMouseDeltaX();
   int deltaMouseY = input->getMouseDeltaY();

   
   float deltaRY = deltaMouseX * m_mouseSensitivity * deltaTime;
   float deltaRX = deltaMouseY * m_mouseSensitivity * deltaTime;
   */

   m_worldTransform = glm::translate(mat4(), moveVector * deltaTime);
   
   m_viewMatrix = glm::inverse(m_worldTransform);
}

void FlyCamera::setSpeed(float spd)
{
   m_moveSpeed = spd;
}

void FlyCamera::setClamp(float clampAngle)
{
   m_clampAngle = clampAngle; 
}

void FlyCamera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up = { 0,1,0 })
{
   m_viewMatrix = lookAt(from, to, up);
   m_up = up;
   updateWorldTransform();
}
