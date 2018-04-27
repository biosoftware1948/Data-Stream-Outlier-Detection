#include "ConnectionManager.hpp"

ConnectionManager::ConnectionManager(){
    this->s = -1;
    this->address = "";
    this->port = 0;
}

bool ConnectionManager::createSocket(){
    /*Check if socket exists */
    if(this->s != -1){
        cerr << "Tried to create socket even though it already exists";
        exit(0);
    }
    /*create socket*/
    this->s = socket(AF_INET, SOCK_STREAM, 0);
    if(this->s == -1){
        cerr << "Failed to create socket ugh";
        exit(0);
    }
    
    return true;
}

bool ConnectionManager::connectByIPv4(){
    server.sin_addr.s_addr = inet_addr(this->address.c_str() );
    server.sin_family = AF_INET;
    server.sin_port = htons(this->port);
    
    /*Connect to server*/
    if (connect(this->s , (struct sockaddr *)&(this->server) , sizeof(this->server)) < 0)
    {
        cerr << "failed to connect to server";
        exit(0);
    }
    
    return true;
}

void ConnectionManager::RecieveMessage(){
    char buf[1024];
    int response = 1;
    /*Recieve messages from server infinitely */
    while(1){
        response = recv(this->s, buf, 1024, 0);
        if (response < 0){
            cerr << "Failure to recieve message";
            exit(0);
        }
        if (response == 0){
            /*Let detector thread know client is disconnected*/
            vector<int> stop_message;
            stop_message.push_back(-1);
            cout << "Client has closed connection, the listener thread will now exit. Take care friend" << endl;
            exit(0);
        }
        
        string s = buf;
        /*remove whitespace*/
        s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
        /*push values into array, then put into buffer for other thread to use*/
        int val;
        vector<int> dataPoint;
        stringstream ss(s);
        while (ss >> val){
            dataPoint.push_back(val);
            if (ss.peek() == ','){
                ss.ignore();
            }
        }
        /*Mutex for buffer grab */
        lock_guard<mutex> guard(bufferLock);
        INPUT_BUFFER.push(dataPoint);
        
        /*flush message buffer and dataPoint placeholder*/
        memset(buf, 0, sizeof(buf));
        dataPoint.clear();
    }
}

bool ConnectionManager::Connect(string address, int port){
    /*Check if hostname, if so get iP address */
    if(!isdigit(address[0]) && !isdigit(address[1])){
    	hostent *rec = gethostbyname(address.c_str());
    	in_addr *addr = (in_addr*)rec->h_addr;
    	address = inet_ntoa(*addr);
    }
    /*Set vars */
    this->address = address;
    this->port = port;
    /*Create socket & Connect */
    this->createSocket();
    this->connectByIPv4();
}

void ExtractAddressPort(string input, string &address, int& port){
    bool colon = false;
    
    for(int i = 0; i < input.size(); ++i){
        if (input[i] == ':'){
            colon = true;
            continue;
        }
        if (!colon){
            address+=input[i];
        }
        else{
            port = atoi(input.substr(i, input.size()-1).c_str());
            return;
        }
    }
}

void SanitizeString(string &s){
    s.erase(remove_if(s.begin(), s.end(), ::isspace), s.end());
    return;
}
