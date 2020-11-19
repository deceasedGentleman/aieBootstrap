#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>

class Camera
{
public:
   Camera();
   Camera(float FOV, float AR, glm::vec3 pos, glm::vec3 lookTo, glm::vec3 up);
   ~Camera();

public:
   virtual void update(float deltaTime) = 0;
   virtual void draw();

   void setPerspective(float FOV, float aspectRatio, float near = 0.1f, float far = 1000.f);
   void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up = { 0,1,0 });
   void setPosition(glm::vec3 position);
   void setWorldTransform(glm::mat4 WT);
   
   glm::vec3 getPosition();
   glm::vec3 getRow(unsigned int row);

   glm::mat4 getWorldTransform();
   glm::mat4 getProjectionView();

   void updateWorld();
   void updateView();

protected:
   // camera transforms
   glm::mat4 m_worldTransform;
   glm::mat4 m_viewMatrix;
   glm::mat4 m_projectionMatrix;
};