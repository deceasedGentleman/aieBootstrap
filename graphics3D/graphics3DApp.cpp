#include "graphics3DApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FlyCamera.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

graphics3DApp::graphics3DApp() {
   m_camera = new FlyCamera();
}

graphics3DApp::~graphics3DApp() {

}

bool graphics3DApp::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

   

	return true;
}

void graphics3DApp::shutdown() {

	Gizmos::destroy();
}

void graphics3DApp::update(float deltaTime) {

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

   m_camera->update(deltaTime);
}

void graphics3DApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

   // update perspective in case window resized
   m_camera->setPerspective(glm::pi<float>() * 0.25f,
                            (float)getWindowWidth() / (float)getWindowHeight(),
                            0.1f, 1000.f);

   m_camera->draw();
}