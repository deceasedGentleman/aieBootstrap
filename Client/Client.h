#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include <string>
#include <unordered_map>
#include <deque>

#include "GameObject.h"

enum Connection
{
   C_CONNECTED,
   C_CONNECTING,
   C_DISCONNECTED,
};

class Client : public aie::Application {
public:

	Client();
	virtual ~Client();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
   void DrawGUI(float deltaTime);
	virtual void draw();
   void HandleInputs(float deltaTime);

   void InitialiseClientConnection();
   void HandleNetworkConnections();

   void SendClientGameObject();
   void SendChatMessage(std::string message);
   void RequestUsername(std::string name);
   void SpawnBullet(glm::vec3 facing, float speed);

   void HandleNetworkMessages();

   void recieveClientGameObject(RakNet::Packet* packet);
   void recieveNetworkMessage(RakNet::Packet* packet);
   void onSetClientIDPacket(RakNet::Packet* packet);
   void onDespawn(RakNet::Packet* packet);

protected:

   RakNet::RakPeerInterface* m_peerInterface;
   //ImGuiIO io;
   const char* IP = "127.0.0.1";
   const unsigned PORT = 5456;
   int m_userID = 0;
   char m_name[8] = "ANON";
   std::deque<std::string> m_chatlog;
   std::unordered_map<int, GameObject> m_otherObjects;
   GameObject m_myObject;
   float m_speed = 10.0f;
   glm::vec3 m_facing = glm::vec3(0);
   Connection m_currentState = C_CONNECTED;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
};