//Packet class for packet wrap up

using namespace std;

class Packet
{
    public:
    char  incomingPayLoad[258];
    struct sockaddr_in client_add;

    Packet(char *payLoad,struct sockaddr_in client_address);
    Packet();
};