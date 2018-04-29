#include "ConnectionManager.hpp"
#include "GridOutlierDetection.hpp"

/*GLOBALS*/
mutex bufferLock;
queue< vector<int> > INPUT_BUFFER;
bool complete = false;

int main(){
    int windowSize;
    string input;
    string address;
    int port;
    /*Get user input of window size, and IPv4:Port */
    cin >> windowSize;
    cin >> input;
    /*remove whitespace, extract IPv4 & Port from input */
    SanitizeString(input);
    ExtractAddressPort(input, address, port);

    ConnectionManager *C = new ConnectionManager();
    GridOutlierDetection * D = new GridOutlierDetection(windowSize);
    C->Connect(address, port);

    thread listener(&ConnectionManager::RecieveMessage, C);
    thread detector(&GridOutlierDetection::DetectOutliers, D);

    listener.join();
    detector.join();
    delete C;
    delete D;
    
    return 0;
    
}
