
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

float m_health = 100;

struct Score
{
   int K;
   int D;
} m_score;

using aie::Gizmos;

Client::Client() { }

Client::~Client() { }

bool Client::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

   HandleNetworkConnections();

   m_myObject = GameObject();
   m_myObject.data.colour = glm::vec4(0.4f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(glm::vec3(0, 10, 10), glm::vec3(0), glm::vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  (float)getWindowWidth() / (float)getWindowHeight(),
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

   // handle the ImGui code
   DrawGUI(deltaTime);

   // parse any messages from the server
   HandleNetworkMessages();

   // handle game inputs if ImGui isn't waiting for typed text
   ImGuiIO io = ImGui::GetIO();
   if (!io.WantTextInput)
   {
      HandleInputs(deltaTime);
   }
   // updates state to server if we have a valid ID
   if (m_myObject.id != 0)
      SendClientGameObject();
}

void Client::DrawGUI(float deltaTime)
{

   ImGui::Text("Server status: "); ImGui::SameLine();
   switch (m_currentState)
   {
   case C_CONNECTED:
      ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected!");
      break;
   case C_CONNECTING:
      ImGui::TextColored(ImVec4(1, 1, 0, 1), "Connecting...");
      break;
   case C_DISCONNECTED:
      ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not Connected");
      ImGui::SameLine();
      if (ImGui::Button("Reconnect?"))
      {
         InitialiseClientConnection();
      }
      break;
   }
   if (ImGui::CollapsingHeader("Chat Program"))
   {
   static std::vector<std::string> chatLog;
   ImGui::BeginChild("Chat Program", ImVec2(ImGui::GetWindowContentRegionWidth(), 300), true);
      ImGui::BeginChild("ChatLog", ImGui::GetContentRegionAvail(), true);
         ImGui::Separator();
         for (int i = 0; i < chatLog.size(); i++)
         {
            ImGui::TextWrapped(chatLog[i].c_str()); //outputs chat log
            ImGui::Separator();
         }
      ImGui::EndChild();
      static bool nameChosen = false;
      if (!nameChosen)
      {
         ImGui::BeginGroup();
            if (ImGui::InputText("Username", m_name, 8, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Set Name"))
            {
               std::cout << "username set to " << m_name << std::endl;
               Client::RequestUsername(m_name);
               nameChosen = true;
            }
         ImGui::EndGroup();
      } else
      {
         ImGui::BeginGroup();
            char tbuf[64] = "";
            if (ImGui::InputText("Chat Input", tbuf, 64, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Send"))
            {
               Client::SendChatMessage(tbuf);
            }
         ImGui::EndGroup();
      } 
   ImGui::EndChild();
   }
   if (ImGui::CollapsingHeader("Game Object"))
   {
      ImGui::BeginGroup();
         ImGui::InputFloat3("position", glm::value_ptr(m_myObject.data.position));
         ImGui::ColorEdit3("colour", glm::value_ptr(m_myObject.data.colour));
      ImGui::EndGroup();
   }
}

void Client::HandleInputs(float deltaTime)
{
   aie::Input* input = aie::Input::getInstance();
   bool stateChanged = false;

   glm::vec3 dface(0);

   // directional movement
   if (input->isKeyDown(aie::INPUT_KEY_UP) || input->isKeyDown(aie::INPUT_KEY_W))
   {
      m_myObject.data.position.z -= m_speed * deltaTime;
      dface.y -= 1;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_DOWN) || input->isKeyDown(aie::INPUT_KEY_S))
   {
      m_myObject.data.position.z += m_speed * deltaTime;
      dface.y += 1;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_LEFT) || input->isKeyDown(aie::INPUT_KEY_A))
   {
      m_myObject.data.position.x -= m_speed * deltaTime;
      dface.x -= 1;
      stateChanged = true;
   }
   if (input->isKeyDown(aie::INPUT_KEY_RIGHT)|| input->isKeyDown(aie::INPUT_KEY_D))
   {
      m_myObject.data.position.x += m_speed * deltaTime;
      dface += 1;
      stateChanged = true;
   }
   if (stateChanged)
   {
      m_facing = dface;
      SendClientGameObject();
   }

   // firing
   if (input->wasKeyPressed(aie::INPUT_KEY_SPACE))
   {
      this->SpawnBullet(m_facing, m_speed * 1.5);
   }

   // quit if we press escape
   if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
      quit();
}


void Client::draw() 
{
   // add own and other spheres
   Gizmos::addSphere(m_myObject.data.position, 1.0f, 32, 32, m_myObject.data.colour);
   for (auto& otherObject : m_otherObjects)
   {
      if (otherObject.first >= 1000)
      { Gizmos::addSphere(otherObject.second.data.position, 0.2f, 8, 8, otherObject.second.data.colour); }
      else 
      { Gizmos::addSphere(otherObject.second.data.position, 1.0f, 32, 32, otherObject.second.data.colour); }
   }

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void Client::HandleNetworkConnections()
{
   m_peerInterface = RakNet::RakPeerInterface::GetInstance();
   InitialiseClientConnection();
}

void Client::InitialiseClientConnection()
{
   m_currentState = C_CONNECTING;
   RakNet::SocketDescriptor sd;

   m_peerInterface->Startup(1, &sd, 1);

   std::cout << "connecting to server at: " << IP << std::endl;

   RakNet::ConnectionAttemptResult result = m_peerInterface->Connect(IP, PORT, nullptr, 0);

   if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
   {
      m_currentState = C_DISCONNECTED;
      std::cout << "Unable to start connection, Error number: " << result << std::endl;
   }
}

void Client::HandleNetworkMessages()
{

   RakNet::Packet* packet;

   for (packet = m_peerInterface->Receive(); packet;
             m_peerInterface->DeallocatePacket(packet),
             packet = m_peerInterface->Receive())
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
         m_currentState = C_CONNECTED;
         
         break;
      case ID_CONNECTION_ATTEMPT_FAILED:
         std::cout << "Our connection attempt has failed. \n";
         m_currentState = C_DISCONNECTED;
         break;
      case ID_NO_FREE_INCOMING_CONNECTIONS:
         std::cout << "The server is full.\n";
         m_currentState = C_DISCONNECTED;
         break;
      case ID_DISCONNECTION_NOTIFICATION:
         std::cout << "We have been disconnected.\n";
         m_currentState = C_DISCONNECTED;
         break; 
      case ID_CONNECTION_LOST:
         std::cout << "We have lost connection.\n";
         m_currentState = C_DISCONNECTED;
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

   if (clientID != m_myObject.id)
   {
      GameObject clientData;
      clientData.Read(packet);
      m_otherObjects[clientID] = clientData;
   }
}

void Client::onSetClientIDPacket(RakNet::Packet* packet)
{
   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
   bsIn.Read(m_myObject.id);
   m_myObject.data.colour = GameObject::GetColour(m_myObject.id);
   std::cout << "Set my Client ID to: " << m_myObject.id << std::endl;
}

void Client::onDespawn(RakNet::Packet * packet)
{

   RakNet::BitStream bsIn(packet->data, packet->length, false);
   bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

   int despawn_id;
   bsIn.Read(despawn_id);
   m_otherObjects.erase(despawn_id);
}

void Client::SendClientGameObject()
{
   if(m_myObject.id != 0)
      m_myObject.Write(m_peerInterface, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::SendChatMessage(std::string message)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHAT_MESSAGE);
   bs.Write(message);
   m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::RequestUsername(std::string name)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CHANGE_USERNAME);
   bs.Write(name);
   m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Client::SpawnBullet(glm::vec3 facing, float speed)
{
   RakNet::BitStream bs;
   bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_SPAWN_BULLET);

   glm::vec3 spawnPos = m_myObject.data.position + facing;
   glm::vec3 bulletVel = facing * speed;

   bs.Write((char*)&spawnPos, sizeof(glm::vec3));
   bs.Write((char*)&bulletVel, sizeof(glm::vec3));

   m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                        RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}



