//UDP Client

class Client
{

  public:
    Client();
    ~Client();
    void send_UDP();
    void sendRequestedSequencepackets_UDP();
    void recv_UDP();

  private:
    int clientSock;
    unsigned int port;
    struct sockaddr_in serverAddress;
    socklen_t addr_size = sizeof(serverAddress);
    char ipaddress[15];
};