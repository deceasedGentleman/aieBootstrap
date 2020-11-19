#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>


#include "graphics3DApp.h"
#include "Gizmos.h"
#include "Input.h"
#include "FlyCamera.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

using aie::Gizmos;

#include <Windows.h>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugString(os_.str().c_str() );  \
}

graphics3DApp::graphics3DApp()
{

}

graphics3DApp::~graphics3DApp()
{}

bool graphics3DApp::startup()
{
    hack = true;
    cursorLock = false;
    glfwSetInputMode(m_window, GLFW_CURSOR, cursorLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    m_camera = new FlyCamera(glm::radians(45.0f), float(16 / 9),
        {10,10,10},
        -0.612456739, -2.35790324);
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
    if (hack)
    {
        hack = false;
    }
    //find the center of the screen
    glm::vec2 mid = { getWindowWidth() / 2, getWindowHeight() / 2 };

    // wipe the gizmos clean for this frame
    Gizmos::clear();

    // draw a simple grid with gizmos
    glm::vec4 white(1);
    glm::vec4 black(0, 0, 0, 1);
    for (int i = 0; i < 21; ++i)
    {
        Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
            glm::vec3(-10 + i, 0, -10),
            i == 10 ? white : black);
        Gizmos::addLine(glm::vec3(10, 0, -10 + i),
            glm::vec3(-10, 0, -10 + i),
            i == 10 ? white : black);
    }

    //draw a cross with radius 'l' in the center of the screen
    float l = 4.0f;
    Gizmos::add2DLine(glm::vec2(mid.x + l, mid.y), glm::vec2(mid.x - l - 1, mid.y), glm::vec4(1));
    Gizmos::add2DLine(glm::vec2(mid.x, mid.y + l), glm::vec2(mid.x, mid.y - l - 1), glm::vec4(1));

    // add a transform so that we can see the axis
    Gizmos::addTransform(glm::mat4(1));

    // clicking on the window locks the cursor, pressing escape unlocks or quits if already unlocked
    aie::Input* input = aie::Input::getInstance();

    if (!cursorLock && input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT))
        cursorLock = true;

    if (cursorLock && glfwGetWindowAttrib(m_window, GLFW_FOCUSED) == false)
    {
        cursorLock = false;
    }

    if (input->wasKeyPressed(aie::INPUT_KEY_ESCAPE))
    {
        if (cursorLock)
            cursorLock = false;
        else if (!cursorLock)
            quit();
    }

    glfwSetInputMode(m_window, GLFW_CURSOR, cursorLock ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    if (glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
    {
        //update the camera's transform
        m_camera->update(deltaTime, input);
    }

    /*DBOUT("pos: (" << float(m_camera->getPosition().x) << ", " << m_camera->getPosition().y "), ");*/
    DBOUT("Camera: (" << m_camera->m_XAngle << "," << m_camera->m_YAngle << ")\n");
    printf("Camera: ( %d, %d )", m_camera->m_XAngle, m_camera->m_YAngle)
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