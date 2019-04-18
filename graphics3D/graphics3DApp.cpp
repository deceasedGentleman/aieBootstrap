#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "graphics3DApp.h"
#include "Gizmos.h"
#include "Input.h"
#include "FlyCamera.h"


using namespace glm;
using aie::Gizmos;

graphics3DApp::graphics3DApp()
{

}

graphics3DApp::~graphics3DApp()
{

}

bool graphics3DApp::startup()
{

   m_camera = new FlyCamera(radians(45.0f), 16 / 9,
                           vec3(10), vec3(0), vec3(0, 1, 0));
   setShowCursor(false);

   setBackgroundColour(0.25f, 0.25f, 0.25f);

   // initialise gizmo primitive counts
   Gizmos::create(10000, 10000, 10000, 10000);
   return true;
}

void graphics3DApp::shutdown()
{

   Gizmos::destroy();
}

void graphics3DApp::update(float deltaTime)
{

   // wipe the gizmos clean for this frame
   Gizmos::clear();

   // draw a simple grid with gizmos
   vec4 white(1);
   vec4 black(0, 0, 0, 1);
   for (int i = 0; i < 21; ++i)
   {
      Gizmos::addLine(vec3(-10 + i, 0, 10),
                  vec3(-10 + i, 0, -10),
                  i == 10 ? white : black);
      Gizmos::addLine(vec3(10, 0, -10 + i),
                  vec3(-10, 0, -10 + i),
                  i == 10 ? white : black);
   }

   //draw a cross in the center of the screen
   glm::vec2 mid = { getWindowWidth() / 2, getWindowHeight() / 2 };
   float l = 4.0f;
   Gizmos::add2DLine(vec2(mid.x + l, mid.y), vec2(mid.x - l -1, mid.y), vec4(1));
   Gizmos::add2DLine(vec2(mid.x, mid.y + l), vec2(mid.x, mid.y - l -1), vec4(1));

   // add a transform so that we can see the axis
   Gizmos::addTransform(mat4(1));


   // quit if we press escape
   aie::Input* input = aie::Input::getInstance();

   if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
      quit();

   m_camera->update(deltaTime);
}

void graphics3DApp::draw()
{

   // wipe the screen to the background colour
   clearScreen();

   // update perspective in case window resized
   m_camera->setPerspective(glm::pi<float>() * 0.25f,
      (float)getWindowWidth() / (float)getWindowHeight(),
                            0.1f, 1000.f);

   //draw 3D gizmos using the camera class
   m_camera->draw();

   // draw 2D gizmos using screen dimensions
   Gizmos::draw2D((float)getWindowWidth(), (float)getWindowHeight());
}