

class ClientPacket
{
    public:
    char payLoad[258];

    ClientPacket(char *data)
    {
        memcpy(&payLoad,data,258);
    }
};