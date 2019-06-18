
#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

RakNet::RakPeerInterface* peerInterface = nullptr;
const unsigned int short PORT = 5456;

void handleNetworkMessages(RakNet::RakPeerInterface* IPeer);

/***********************************************/
int main()
{

   std::cout << "Starting up the server..." << std::endl;

   //get an instance of the peer interface
   peerInterface = RakNet::RakPeerInterface::GetInstance();

   //create a descriptor for this socket
   RakNet::SocketDescriptor sd(PORT, 0);

   peerInterface->Startup(32, &sd, 1);
   peerInterface->SetMaximumIncomingConnections(32);

	
	return 0;
}

/////////////////////////////////////
// Handles each packet recieved
/////////////////////////////////////
void handleNetworkMessages(RakNet::RakPeerInterface* IPeer)
{
   RakNet::Packet* packet = nullptr;

   while (true)
   {
      for (packet = IPeer->Receive(); packet;
           IPeer->DeallocatePacket(packet),
           packet = IPeer->Receive())
      {
         switch (packet->data[0])
         {
         case ID_NEW_INCOMING_CONNECTION:
            std::cout << "A connection is incoming. \n";
            break;
         case  ID_DISCONNECTION_NOTIFICATION:
            std::cout << "A client has disconnected. \n";
            break;
         case ID_CONNECTION_LOST:
            std::cout << "A client lost connection \n";
            break;
         default:
            std::cout << "Recieved a message with an unknown id: "
               << packet->data[0];
            break;
         }
      }
   }
}