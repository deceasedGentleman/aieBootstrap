// local files
#include "Client.h"

// AIE files
#include "Gizmos.h"
#include "Input.h"
#include "GameMessages.h"

// Third Party dependencies
#include <imgui.h>
#include <imgui_glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>

using aie::Gizmos;

Client::Client() {

}

Client::~Client() {
}

bool Client::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

   Client::handleNetworkConnections();

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

   _gameObject.position = glm::vec3(0);
   _gameObject.colour = glm::vec4(1, 0, 0, 1);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	return true;
}

void Client::shutdown() {

	Gizmos::destroy();
}

void Client::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

   ImGui::Text("Networking test");
   handleNetworkMessages();

   // messaging removed for the time being
/*
   ImGui::BeginChild("Messaging");
   if (ImGui::InputText("Username", _name.c_str, 8, ImGuiInputTextFlags_EnterReturnsTrue))
   {

   }
      ImGui::BeginChild("ChatLog", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);
      for (int i = 0; i < 100; i++)
      {
         ImGui::Text("NAME: message");
      }
      ImGui::EndChild();
   ImGui::SameLine();
   ImGui::te
   if(ImGui::Button(""))
*/
   handleInputs(deltaTime);

   ImGui::BeginGroup();
   ImGui::Text("game object");
   ImGui::InputFloat3("position", glm::value_ptr(_gameObject.position));
   ImGui::ColorEdit3("colour", glm::value_ptr(_gameObject.colour));
   ImGui::EndGroup();

   Gizmos::addSphere(_gameObject.position, 1.0f, 32, 32, _gameObject.colour);

   sendClientGameObject();

}

void Client::handleInputs(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   bool stateChanged = false;

   if (input->isKeyDown(aie::INPUT_KEY_LEFT))
   {
      _gameObject.position.x -= 10.0f * deltaTime;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
   {
      _gameObject.position.x += 10.0f * deltaTime;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_UP))
   {
      _gameObject.position.z += 10.0f * deltaTime;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_DOWN))
   {
      _gameObject.position.z -= 10.0f * deltaTime;
      stateChanged = true;
   }

   if(stateChanged)
      sendClientGameObject();

   // quit if we press escape
   if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
      quit();
}

void Client::draw() 
{

   for (auto& otherClient : _otherClients)
   {
      Gizmos::addSphere(otherClient.second.position,
                        1.0f, 32, 32, otherClient.second.colour);
   }

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void Client::handleNetworkConnections()
{
   _peerInterface = RakNet::RakPeerInterface::GetInstance();
   initialiseClientConnection();
}

void Client::initialiseClientConnection()
{
   RakNet::SocketDescriptor sd;

   _peerInterface->Startup(1, &sd, 1);

   std::cout << "connecting to server at: " << IP << std::endl;

   RakNet::ConnectionAttemptResult result = _peerInterface->Connect(IP, PORT, nullptr, 0);

   if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
   {
      std::cout << "Unable to start connection, Error number: " << result << std::endl;
   }

}

void Client::handleNetworkMessages()
{

   RakNet::Packet* packet;

   for (packet = _peerInterface->Receive(); packet;
             _peerInterface->DeallocatePacket(packet),
             packet = _peerInterface->Receive())
   {
      switch (packet->data[0])
      {
      case ID_REMOTE_DISCONNECTION_NOTIFICATION:
         std::cout << "Another client has disconnected.\n";
         break;
      case ID_REMOTE_CONNECTION_LOST:
         std::cout << "Another client has lost connection.\n";
         break;
      case ID_CONNECTION_REQUEST_ACCEPTED:
         std::cout << "Our connection request has been accepted.\n";
         break;
      case ID_CONNECTION_ATTEMPT_FAILED:
         std::cout << "Our connection attempt has failed. \n";
         break;
      case ID_NO_FREE_INCOMING_CONNECTIONS:
         std::cout << "The server is full.\n";
         break;
      case ID_DISCONNECTION_NOTIFICATION:
         std::cout << "We have been disconnected.\n";
         break; 
      case ID_CONNECTION_LOST:
         std::cout << "We have lost connection.\n";
         break; 
      case ID_SERVER_SET_CLIENT_ID:
         onSetClientIDPacket(packet);
         break;
      case ID_SERVER_TEXT_MESSAGE: 
      case ID_CLIENT_CHAT_MESSAGE:
         recieveNetworkMessage(packet);
         break;
      case ID_CLIENT_OBJECT_DATA:
         recieveClientGameObject(packet);
         break;
      default:
         std::cout << "Received message with unknown id: " << (DefaultMessageIDTypes)packet->data[0] << std::endl;
         break;
      }
   }
}

void Client::recieveNetworkMessage(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   RakNet::RakString str;
   bsIn.Read(str);
   std::cout << str << std::endl;
}

void Client::recieveClientGameObject(RakNet::Packet * packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   int clientID;
   bsIn.Read(clientID);

   if (clientID != _userID)
   {
      GameObject clientData;
      bsIn.Read((char*)&clientData, sizeof(GameObject));

      _otherClients[clientID] = clientData;

      //// debug message
      //std::cout << "[DBG] Client " << clientID << " at: (" << 
      //   clientData.position.x << ", " << clientData.position.z << ")" << std::endl;
   }
}

void Client::onSetClientIDPacket(RakNet::Packet* packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   bsIn.Read(_userID);
   std::cout << "Set my Client ID to: " << _userID << std::endl;
}

void Client::sendClientGameObject()
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_OBJECT_DATA);
   bs.Write(_userID);
   bs.Write((char*)&_gameObject, sizeof(GameObject));

   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::requestUsername(std::string name)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHANGE_USERNAME);
   bs.Write(_name);
   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}



