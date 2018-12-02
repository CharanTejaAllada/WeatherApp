//Server packets processor
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <deque>
#include <math.h>
#include <mutex>
#include <condition_variable>
#include "ServerPackets.h"

//Global varibales
unordered_map<string, string> id_Holder;
int packetCount = 0;
map<int, string> received_data;
ofstream dataOut, packOut;
deque<Packet> packet_queue;
std::mutex packet_Mutex, recvDataMutex;
std::condition_variable packet_CV;
vector<int> sequenceNumberRequests;
bool flag = false;
bool doneTransfer = false;

Packet::Packet(char *payLoad, struct sockaddr_in client_address)
{
    memcpy(&incomingPayLoad, payLoad, 258);
    this->client_add = client_address;
}

Packet::Packet()
{
    this->client_add.sin_family = AF_INET;
    this->client_add.sin_port = htons(0000);
    this->client_add.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(this->client_add.sin_zero, '0', sizeof(this->client_add.sin_zero));
}


//Write to file when a empty buffer packet is sent
void write_to_file()
{
    dataOut.open("out.txt", ios::app);
    auto it = received_data.begin();

    for (auto it : received_data)
    {
        dataOut << it.first;
        dataOut << " ";
        dataOut << it.second;
        dataOut << "\n";
    }

    received_data.erase(received_data.begin(), received_data.end());
    dataOut.close();
}

//Server debugging the packet numbers
void packet_info()
{
    while (1)
    {
        cout << "Packets received till now " << packetCount << endl;
    }
}


//Whole processing of the queue
void packet_processor()
{
    bool flip = true;
    while (!doneTransfer)
    {
        Packet *tmp = new Packet();
        if (!packet_queue.empty())
        {
            {
                std::unique_lock<std::mutex> locker(packet_Mutex);
                memcpy(&(tmp->incomingPayLoad), (char *)&(packet_queue.back().incomingPayLoad), 258);
                tmp->client_add = packet_queue.back().client_add;
                packet_queue.pop_back();
            }

            unsigned short int tempSeq;
            char data[255];
            char *ptr = tmp->incomingPayLoad;
            memcpy(&tempSeq, ptr, 2);
            ptr = ptr + 2;
            if (flip)
            {
                memcpy(&flag, ptr, 1);
                if (flag == true)
                {
                    flip = false;
                }
            }
            ptr++;
            memcpy(&data, ptr, 255);

            {
                std::unique_lock<mutex> locker(recvDataMutex);
                received_data.insert(make_pair(tempSeq, data));
            }

            cout << "Sequence Number is : " << tempSeq << endl;
            cout << "Data is: " << data << endl;
            cout << "Acknowledgement bit : " << flag << endl
                 << endl
                 << endl;
        }
        delete tmp;
    }
    cout<<"Packet processor exited\n";
}