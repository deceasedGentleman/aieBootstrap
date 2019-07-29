#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <string>
#include <unordered_map>

#include "GameObject.h"

class Client : public aie::Application {
public:

	Client();
	virtual ~Client();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();
   void HandleInputs(float deltaTime);

   static void Chat(Client* client);

   void initialiseClientConnection();
   void handleNetworkConnections();

   void sendClientGameObject();
   void sendChatMessage(std::string message);
   void requestUsername(std::string name);
   void SpawnBullet();

   void HandleNetworkMessages();

   void recieveClientGameObject(RakNet::Packet* packet);
   void recieveNetworkMessage(RakNet::Packet* packet);
   void onSetClientIDPacket(RakNet::Packet* packet);
   void onDespawn(RakNet::Packet* packet);

protected:

   RakNet::RakPeerInterface* _peerInterface;
   const char* IP = "127.0.0.1";
   const unsigned PORT = 5456;
   int _userID = 0;
   std::string _name = "ANON";
   std::unordered_map<int, GameObject> _otherObjects;
   GameObject _myObject;
   glm::vec3 _facing;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};