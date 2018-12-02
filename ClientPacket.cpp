//Client packets processor
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <map>
#include <math.h>
#include <mutex>
#include <condition_variable>
#include "ClientPacket.h"

using namespace std;

static int packet_count = 0;
std::mutex client_mutex;
ifstream serverPacks;
std::map<int,ClientPacket> prepared_packets;
std::vector<unsigned short int> requestedSequenceNumbers;

void wrapDataIntoPackets(string fileName)
{
    ifstream fileOpen;
    unsigned short int seq_number = 0;
    int numberOfPacketsToSend = 0;
    std::string line;

    std::ifstream myfile(fileName);
    while (std::getline(myfile, line))
        ++numberOfPacketsToSend;

    //Prepare the file system
    fileOpen.open(fileName);
    for (int i = 0; i < numberOfPacketsToSend; i++)
    {
        char *payLoad = new char[258];
        char *data = &payLoad[3];
        char *flag = &payLoad[2];
        fileOpen.getline(data, 255);
        seq_number++;
        memcpy(payLoad, (char *)&seq_number, sizeof(unsigned short int));
        memset(flag, '0', 1);
        if (i == numberOfPacketsToSend - 1)
        {
            memset(flag, '1', 1);
        }
        prepared_packets.insert(make_pair(seq_number, ClientPacket(payLoad)));
    }
    fileOpen.close();
}

void unZipPackets()
{
    for (auto it : prepared_packets)
    {
        char buffer[258];
        memcpy(&buffer[0], &(it.second), 258);
        //
        unsigned short int testSeq;
        char testBuffer[255];
        bool flag;

        memcpy(&testSeq, &buffer[0],2);
        memcpy(&testBuffer, &buffer[3], 255);
        memcpy(&flag, &buffer[2], 1);
        cout << "Sq number: " << testSeq << endl;
        cout << " Data: " << testBuffer << endl;
        cout << " Acknowledgement bit is: " << flag << endl;
    }
}


