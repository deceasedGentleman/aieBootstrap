#pragma once

#include "Gizmos.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Camera
{
public:
   Camera();
   ~Camera();

public:
   virtual void update(float deltaTime) = 0;
   virtual void draw();

   void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
   void updateWorldTransform();
   void setLookAt(glm::vec3 from, glm::vec3 to, glm::vec3 up);
   void setPosition(glm::vec3 position);

   glm::mat4 getProjectionView();

protected:
   // camera transforms
   glm::mat4 m_worldTransform;
   glm::mat4 m_viewMatrix;
   glm::mat4 m_projectionMatrix;
   glm::mat4 m_projectionView;

   // pretty self-explanatory
   void updateProjectionView();
};