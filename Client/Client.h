#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <string>
#include <unordered_map>


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
   void handleInputs(float deltaTime);

   void initialiseClientConnection();
   void handleNetworkConnections();

   void sendClientGameObject();
   void requestUsername(std::string name);

   void handleNetworkMessages();

   void recieveClientGameObject(RakNet::Packet* packet);
   void onSetClientIDPacket(RakNet::Packet* packet);
   void recieveNetworkMessage(RakNet::Packet* packet);

protected:

   RakNet::RakPeerInterface* _peerInterface;
   const char* IP = "127.0.0.1";
   const unsigned PORT = 5456;
   int _userID = 0;
   std::string _name = "ANON";
   std::unordered_map<int, GameObject> _otherClients;
   GameObject _gameObject;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};