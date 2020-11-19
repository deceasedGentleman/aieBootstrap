#pragma once
#include "Application.h"
#include "FlyCamera.h"

class graphics3DApp : public aie::Application {
public:

	graphics3DApp();
	virtual ~graphics3DApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();
private:
   FlyCamera* m_camera;
   bool cursorLock;
   bool hack;
};