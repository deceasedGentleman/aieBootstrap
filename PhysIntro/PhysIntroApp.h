#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "PhysicsScene.h"

class PhysIntroApp : public aie::Application {
public:

	PhysIntroApp();
	virtual ~PhysIntroApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void setupContinuousDemo(glm::vec2 startpos, float speed,
									 float angle);

protected:

	PhysicsScene*		m_physicsScene;
	aie::Renderer2D*	m_2dRenderer;
	aie::Font*			m_font;
};