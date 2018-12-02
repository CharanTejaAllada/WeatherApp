
//UDP Server class
#include "ServerPackets.cpp"

class Server
{
    //fields
    int serverSock;
    unsigned int port;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t addr_sizeClient = sizeof(clientAddress);

  public:
    Server(int);
    ~Server();
    void receive_UDP();
    void send_UDP();
};