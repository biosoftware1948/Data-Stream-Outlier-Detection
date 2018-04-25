#ifndef connector_h
#define connector_h

#include "Globals.hpp"

using namespace std;

class ConnectionManager{
private:
    int s;
    string address;
    int port;
    struct sockaddr_in server;
    
    bool createSocket();
    bool connectByIPv4();
    
public:
    ConnectionManager();
    bool Connect(string address, int);
    bool SendData(string data);
    void RecieveMessage();
    void Disconnect();
};

#endif
