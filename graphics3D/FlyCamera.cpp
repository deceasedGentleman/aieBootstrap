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
   glm::vec3 deltaTranslate = vec3(0);
   glm::vec3 deltaMouse = vec3(0);

   //horizontal movement
   if (input->isKeyDown(aie::INPUT_KEY_W))
      deltaTranslate.z += 1;
   if (input->isKeyDown(aie::INPUT_KEY_A))
      deltaTranslate.x += 1;
   if (input->isKeyDown(aie::INPUT_KEY_S))
      deltaTranslate.z -= 1;
   if (input->isKeyDown(aie::INPUT_KEY_D))
      deltaTranslate.x -= 1;

   //vertical movement
   if (input->isKeyDown(aie::INPUT_KEY_SPACE))
      deltaTranslate.y += 1;
   if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT))
      deltaTranslate.y -= 1;

   int deltaMouseX = input->getMouseDeltaX();
   int deltaMouseY = input->getMouseDeltaY();

   //we'll do rotation after
   //float deltaRY = deltaMouseX * m_mouseSensitivity * deltaTime;
   //float deltaRX = deltaMouseY * m_mouseSensitivity * deltaTime;


   m_worldTransform = glm::translate(m_worldTransform, deltaTranslate);
   m_viewMatrix = glm::inverse(m_worldTransform);
}

void FlyCamera::setSpeed(float spd)
{
   m_moveSpeed = spd;
}
