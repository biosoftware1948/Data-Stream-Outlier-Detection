#ifndef connector_h
#define connector_h

#include "Globals.hpp"

using namespace std;

class ConnectionManager{
private:
    /*
     *s holds socket number
     */
    int s;
    /*
     *iP address of client
     */
    string address;
    /*
     *Port number of client
     */
    int port;
    /*
     *network struct to help
     */
    struct sockaddr_in server;
    /*
     *Function creates socket
     */
    bool createSocket();
    /*
     *Connects us to the client
     */
    bool connectByIPv4();
    
public:
    /*
     *Constructor
     *Sets s=-1 and port=0, address="" so we can check if a connection is already made.
     */
    ConnectionManager();
    /*
     *Connects us to the client
     *PARAM: Address can be either the IP address or local hostname of the client
     *PARAM: port is the port number to connect to
     */
    bool Connect(string address, int port);
    /*
     *Recieves messages from the Client, runs indefinitely. When a message is recieved it is
     converted to an array and pushed to a buffer.
     *
     *ALTERS GLOBAL INPUT_BUFFER, grabs a mutex before doing so.
     */
    void RecieveMessage();
};

#endif
