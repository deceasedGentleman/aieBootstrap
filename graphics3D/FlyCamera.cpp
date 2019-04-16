#include "FlyCamera.h"
#include "Input.h"

using namespace glm;

FlyCamera::FlyCamera()
{}

FlyCamera::FlyCamera(float FOV, float AR, glm::vec3 pos, glm::vec3 lookTo, glm::vec3 up):
   Camera(FOV, AR, pos, lookTo, up)
{
   m_up = up;
   
}

FlyCamera::~FlyCamera()
{}

void FlyCamera::update(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   MouseLook(deltaTime, input);
   WASDMove(deltaTime, input);
}

void FlyCamera::WASDMove(float deltaTime, aie::Input * input)
{
   
   mat4 currentTransform = getWorldTransform();
   vec3 left = currentTransform[0];
   currentTransform[1] = vec4(m_up, 0);
   vec3 forward = currentTransform[2];
   vec3 currentPos = currentTransform[3];
   //horizontal movement
   if (input->isKeyDown(aie::INPUT_KEY_W))
      currentPos -= forward * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_S))
      currentPos += forward * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_A))
      currentPos -= left * deltaTime * m_moveSpeed;
   if (input->isKeyDown(aie::INPUT_KEY_D))
      currentPos += left * deltaTime * m_moveSpeed;

   //vertical movement 
   if (input->isKeyDown(aie::INPUT_KEY_SPACE)) 
   {
      currentPos += m_up * deltaTime * m_moveSpeed;
   }
   if (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT)) 
   {
      currentPos -= m_up * deltaTime * m_moveSpeed;
   }
   setPosition(currentPos);
}

void FlyCamera::MouseLook(float deltaTime, aie::Input * input)
{
   mat4 currentTransform = getWorldTransform();
   currentTransform[3] = vec4(0, 0, 0, 1);

   //rotation
   double deltaMX = input->getMouseDeltaX();
   double deltaMY = input->getMouseDeltaY();
   float deltaX = clamp((float)deltaMX, -10.0f, 10.0f) * m_MouseSensitivity * deltaTime;
   float deltaY = clamp((float)deltaMY, -10.0f, 10.0f) * m_MouseSensitivity * deltaTime;

   glm::mat4 rotation(1.0f);
   rotation = glm::rotate(rotation, deltaY, getRow(0));
   rotation = glm::rotate(rotation, deltaX, { 0,1,0 });
   currentTransform = rotation * currentTransform;

   currentTransform[3] = vec4(getPosition(), 1);
   setWorldTransform(currentTransform);
}

void FlyCamera::setSpeed(float spd)
{
   m_moveSpeed = spd;
}

void FlyCamera::setClamp(float clampAngle)
{
   m_clampAngle = clampAngle; 
}

void FlyCamera::setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up)
{
   m_viewMatrix = lookAt(from, to, up);
}
