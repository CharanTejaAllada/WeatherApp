//UDP server

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Server.h"

Server::Server(int port)
{
    this->port = port;

    if ((serverSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cerr << "Socket creation failed!" << endl;
    }
    cout << "Socket creation successful!" << endl;

    //Preparing Server sockaddr_in
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(serverAddress.sin_zero, '0', sizeof(serverAddress.sin_zero));

    // Binding Socket with port
    if (::bind(serverSock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        cerr << "Binding Failed" << endl;
    }
    else
    {
        cout << "Bind success!" << endl;
    }
}

Server::~Server()
{
    close(serverSock);
    cout << "Connection closed at server" << endl;
}

Server UDP_Server(8811);

void req_Packets()
{
    UDP_Server.send_UDP();
    doneTransfer = true;
    cout<<"Request thread exited\n";
}

void Server::receive_UDP()
{
    char packet[258];

    while (!doneTransfer)
    {
        if (recvfrom(serverSock, packet, sizeof(packet), 0, (struct sockaddr *)&clientAddress, &addr_sizeClient) != -1)
        {
            std::unique_lock<std::mutex> locker(packet_Mutex);
            packet_queue.push_front(Packet(packet, clientAddress));
            packet_CV.notify_all();
            packetCount++;
        }
    }

    cout<<"Receive thread exited\n";
}


void Server::send_UDP()
{
   bool allPackets = false;
    while (!flag || !allPackets)
    {
        {
            std::unique_lock<mutex> locker(recvDataMutex);
            //put a mutex here
            auto it = received_data.end();
            it--;
            int size = received_data.size();

            if ((*it).first == size)
            {
                allPackets = true;
            }

            else
            {
                allPackets = false;
                for (int i = 1; i <= size; i++)
                {
                    if (auto tmp = received_data.find(i) == received_data.end())
                    {
                        char packet[2];
                        memcpy(&packet, &i, 2);
                        sendto(serverSock, packet, sizeof(packet), 0, (struct sockaddr *)&clientAddress, addr_sizeClient);
                    }
                }
            }
            
        }//lock dies

        usleep(5000);
    }
}


int main()
{
    //Create a separate thread for processing packets
    std::thread packet_thread(packet_processor);
    std::thread send_thread(req_Packets);

    //Create a server

    //Start receiving packets from clients
    UDP_Server.receive_UDP();

    packet_thread.join();
    send_thread.join();

    return 0;
}
