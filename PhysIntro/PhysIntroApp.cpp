#define _USE_MATH_DEFINES
#include "PhysIntroApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"
#include <glm\ext.hpp>
#include <Gizmos.h>
#include "PhysicsScene.h"
#include "RigidBody.h"
#include "Plane.h"


using namespace aie;
using namespace glm;

vec2 randomPos()   { return linearRand(vec2(80, 40), vec2(-80, -40)); }
vec2 randomVel()   { return linearRand(vec2(40, 40), vec2(-40, -40)); }
float randomMass() { return linearRand(80.f, 200.f); }
float randomSize() { return linearRand(1.5f, 7.f); }
vec4 randomColour(){ return linearRand(vec4(0, 0, 0, 1), vec4(1, 1, 1, 1)); }



PhysIntroApp::PhysIntroApp() {

}

PhysIntroApp::~PhysIntroApp() {

}

bool PhysIntroApp::startup() {
	//increasing maximum 2d line count
	Gizmos::create(255U, 255U, 65535U, 65535U);

	m_2dRenderer = new aie::Renderer2D();
	m_font = new aie::Font("../bin/font/consolas.ttf", 32);

	m_physicsScene = new PhysicsScene();
	m_physicsScene->setGravity(vec2(0, -9.8));
	m_physicsScene->setTimeStep(0.02f);

	m_physicsScene->addActor(new Sphere(
		vec2(-50, 1), vec2(5, 0), 240, 0.8f, 3.0f, glm::vec4(1, 1, 1, 1))
	);
	m_physicsScene->addActor(new Plane(vec2( 1, 0), 90));
	m_physicsScene->addActor(new Plane(vec2(-1, 0), 90));
	m_physicsScene->addActor(new Plane(vec2( 0, 1), 50));
	m_physicsScene->addActor(new Plane(vec2( 0,-1), 50));
	m_physicsScene->addActor(new Sphere(vec2(40, 0), vec2(0, 0), 140, 0.8f, 4.0f, glm::vec4(0, 1, 0, 1)));
	

	return true;
}

void PhysIntroApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PhysIntroApp::update(float deltaTime) {

	// input example
	aie::Input* input = aie::Input::getInstance();

	Gizmos::clear();

	m_physicsScene->update(deltaTime);
	m_physicsScene->updateGizmos();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
	if (input->isKeyDown(aie::INPUT_KEY_SPACE))
	{
		m_physicsScene->addActor(new Sphere(
			randomPos(), randomVel(), randomMass(), 0.8f, randomSize(), randomColour()));
		
	}
}

void PhysIntroApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw your stuff here!
	static float aspectRatio = 16 / 9.f;
	Gizmos::draw2D(ortho<float>(-100, 100,
								-100 / aspectRatio, 100 / aspectRatio, -1.0f, 1.0f));

	// output some text, uses the last used colour
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

void PhysIntroApp::setupContinuousDemo(glm::vec2 startpos, float speed, float angle)
{
	vec2 gravity = m_physicsScene->getGravity();
	float tStep = m_physicsScene->getTimeStep();
	float radius = 1.0f;
	int segments = 12;
	vec4 colour = vec4(1, 1, 0, 1);
	vec2 startvel(speed * cosf(angle), speed * sinf(angle));

	for( float t = 0; t <= 10; t += tStep)
	{
		vec2 pos = startpos + startvel * t + (0.5f * gravity * powf(t, 2));

		Gizmos::add2DCircle(pos, radius, segments, colour);
	}
}

