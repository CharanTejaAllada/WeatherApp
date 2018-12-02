//UDP Client
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <iomanip>
#include <vector>
#include "ClientPacket.cpp"
#include "Client.h"

Client::Client()
{
    //Socket creation
    cout << "Client starting......." << endl;

    if ((clientSock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        cerr << "Socket creation failed!" << endl;
    }
    cout << "Socket creation successful!" << endl;

    //Read Server address and port
    cout << "Enter IP of server to connect" << endl;
    //cin >> ipaddress;
    cout << "Enter the port number of server" << endl;
    //cin >> port;
    port = 8811;
    //Preparing server sockaddr_in
    serverAddress.sin_family = PF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr("10.1.10.235");
    memset(serverAddress.sin_zero, '0', sizeof(serverAddress.sin_zero));
}

void Client::send_UDP()
{
    bool flag = true;
    for (auto it : prepared_packets)
    {
        if (flag)
        {
            if (sendto(clientSock, it.second.payLoad, sizeof(it.second.payLoad), 0, (struct sockaddr *)&serverAddress, addr_size) != -1)
            {
                packet_count++;
            }
            else
            {
                cerr << "Sending failed!" << endl;
            }
            flag = false;
        }

        else
        {
            flag = true;
        }
    }
}

void Client::sendRequestedSequencepackets_UDP()
{
    for (auto itr : requestedSequenceNumbers)
    {
        auto it2 = prepared_packets.find(itr);
        if (it2 != prepared_packets.end())
        {
            sendto(clientSock, (*it2).second.payLoad, sizeof(((*it2).second.payLoad)), 0, (struct sockaddr *)&serverAddress, addr_size);
        }
        else
        {
            cout << "No luck finding!!" << endl;
        }
    }
}

void Client::recv_UDP()
{
    while(1)
    {
        char seqReq[2];
        unsigned short int seq;

        if (recvfrom(clientSock, seqReq, sizeof(seqReq), 0, (struct sockaddr *)&serverAddress, &addr_size) != -1)
        {
            memcpy(&seq, &seqReq, 2);
            requestedSequenceNumbers.push_back(seq);
            cout << "Server requested for the packet with sequence number: " << seq << endl;
            auto it = prepared_packets.find(seq);
            if(it!= prepared_packets.end())
            {
            sendto(clientSock, (*it).second.payLoad, sizeof(((*it).second.payLoad)), 0, (struct sockaddr *)&serverAddress, addr_size);
            }

            else
            {
                cout<<"No luck finding"<<endl;
            }
        }

        else
        {
            cout << "Error receiving server's response" << endl;
        }
    }
   // sendRequestedSequencepackets_UDP();
}

Client::~Client()
{
    close(clientSock);
    cout << "Connection closed at client" << endl;
}

//Client object declared global.......

Client abc;

void sendPacksToServer()
{
    abc.send_UDP();
}

void receivePacksFromServer()
{
    abc.recv_UDP();
}

int main()
{
   
    wrapDataIntoPackets("Lines.txt");
    std::thread send_thread(sendPacksToServer);
    std::thread recv_thread(receivePacksFromServer);
    send_thread.join();
    recv_thread.join();

    return 0;
}
