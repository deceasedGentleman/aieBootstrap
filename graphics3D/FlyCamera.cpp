#include "FlyCamera.h"
#include "Input.h"

using namespace glm;

FlyCamera::FlyCamera()
{
   m_up = { 0,1,0 };
}

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

   //vertical movement 
   /*
   if (input->isKeyDown(aie::INPUT_KEY_SPACE))
   if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))

   int deltaMouseX = input->getMouseDeltaX();
   int deltaMouseY = input->getMouseDeltaY();

   
   float deltaRY = deltaMouseX * m_mouseSensitivity * deltaTime;
   float deltaRX = deltaMouseY * m_mouseSensitivity * deltaTime;
   */

   m_worldTransform = glm::translate(m_worldTransform, moveVector);
   m_viewMatrix = glm::inverse(m_worldTransform);
}

void FlyCamera::setSpeed(float spd)
{
   m_moveSpeed = spd;
}
