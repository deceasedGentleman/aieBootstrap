#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>


struct GameObject
{
   glm::vec3 position;
   glm::vec4 colour;
};

class Client : public aie::Application {
public:

	Client();
	virtual ~Client();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

   void handleNetworkConnections();
   void initialiseClientConnection();
   void handleNetworkMessages();
   void onSetClientIDPacket(RakNet::Packet* packet);
   void requestUsername(const char name[16]);

protected:

   RakNet::RakPeerInterface* _peerInterface;
   const char* IP = "127.0.0.1";
   const unsigned PORT = 5456;
   int USER_ID = 0;
   char _name[16] = "ANON";
   GameObject _gameObject;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};