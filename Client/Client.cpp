
// local files
#include "Client.h"
#include "GameMessages.h"
#include "GameObject.h"

// AIE files
#include "Gizmos.h"
#include "Input.h"

// Third Party dependencies
#include <imgui.h>
#include <imgui_glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <thread>

using aie::Gizmos;

Client::Client() {

}

Client::~Client() {
}

bool Client::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

   Client::handleNetworkConnections();

   _myObject.id = 0;
   _myObject.data.position = glm::vec3(0, 0, 0);
   _myObject.data.colour = glm::vec4(1, 0, 0, 1);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(glm::vec3(10), glm::vec3(0), glm::vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

   //std::thread chatThread(Chat, this);

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
   HandleNetworkMessages();

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

   HandleInputs(deltaTime);

   ImGui::BeginGroup();
   ImGui::Text("game object");
   ImGui::InputFloat3("position", glm::value_ptr(_myObject.data.position));
   ImGui::ColorEdit3("colour", glm::value_ptr(_myObject.data.colour));
   ImGui::EndGroup();

   
   if (_myObject.id != 0)
   sendClientGameObject();

}

void Client::HandleInputs(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   bool stateChanged = false;

   // directional movement
   if (input->isKeyDown(aie::INPUT_KEY_LEFT))
   {
      _myObject.data.position.x -= 10.0f * deltaTime;
      _facing = glm::vec3(-1, 0, 0);
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
   {
      _myObject.data.position.x += 10.0f * deltaTime;
      _facing = glm::vec3(1, 0, 0);
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_UP))
   {
      _myObject.data.position.z -= 10.0f * deltaTime; 
      _facing = glm::vec3(0, -1, 0);
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_DOWN))
   {
      _myObject.data.position.z += 10.0f * deltaTime;
      _facing = glm::vec3(0, 1, 0);
      stateChanged = true;
   }

   // firing
   if (input->wasKeyPressed(aie::INPUT_KEY_SPACE))
   {
      this->SpawnBullet();
   }

   if(stateChanged)
      sendClientGameObject();

   // quit if we press escape
   if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
      quit();
}

///////////////////////////////////////////////////////////////////////////////
// work on chat functionality stalled due to me not understanding strings
///////////////////////////////////////////////////////////////////////////////
void Client::Chat(Client* client)
{
   while (true)
   {
      std::string input;
      std::getline(std::cin, input, ' ');
      if (input == "help")
      {
         std::cin.ignore();
         std::cout << "available commands are:\n"
            << "help: lists commands\n"
            << "say [text]: sends a text message to the other clients\n"
            << "name [text]: changes your client name to [text]\n"
            << std::endl;
      } 
      else if (input == "say")
      {
         if (std::getline(std::cin, input))
         {
            client->sendChatMessage(input);
         }
      }
      else if (input == "name")
      {
         if (std::getline(std::cin, input))
         {
            client->requestUsername(input);
         }
      }
      else
      {
         std::cout << "command not recognised, type 'help' for a list of options" << std::endl;
      }
   }
}

void Client::draw() 
{
   // add own and other spheres
   Gizmos::addSphere(_myObject.data.position, 1.0f, 32, 32, _myObject.data.colour);
   for (auto& otherClient : _otherObjects)
   {
      if (otherClient.first >= 1000)
      { Gizmos::addSphere(otherClient.second.data.position, 0.2f, 8, 8, otherClient.second.data.colour); }
      else 
      { Gizmos::addSphere(otherClient.second.data.position, 1.0f, 32, 32, otherClient.second.data.colour); }
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

void Client::HandleNetworkMessages()
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
      case ID_SERVER_DESPAWN:
         onDespawn(packet);
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

   if (clientID != _myObject.id)
   {
      GameObject clientData;
      clientData.Read(packet);
      _otherObjects[clientID] = clientData;
   }
}

void Client::onSetClientIDPacket(RakNet::Packet* packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   bsIn.Read(_myObject.id);
   _myObject.data.colour = GameObject::GetColour(_myObject.id);
   std::cout << "Set my Client ID to: " << _myObject.id << std::endl;
}

void Client::onDespawn(RakNet::Packet * packet)
{

   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   int despawn_id;
   bsIn.Read(despawn_id);
   _otherObjects.erase(despawn_id);
}

void Client::sendClientGameObject()
{
   if(_myObject.id != 0)
      _myObject.Write(_peerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::sendChatMessage(std::string message)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHAT_MESSAGE);
   bs.Write(message);
   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::requestUsername(std::string name)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHANGE_USERNAME);
   bs.Write(name);
   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::SpawnBullet()
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_SPAWN_BULLET);

   glm::vec3 spawnPos = _myObject.data.position + _facing;

   bs.Write((char*)&spawnPos, sizeof(glm::vec3));
   bs.Write((char*)&_facing, sizeof(glm::vec3));

   _peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}



