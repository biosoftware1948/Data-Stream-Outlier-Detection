#include "ConnectionManager.hpp"
#include "GridOutlierDetection.hpp"
mutex bufferLock;
queue< vector<int> > INPUT_BUFFER;

int main(){
    int windowSize;
    string input;
    string address;
    int port;
    
    cin >> windowSize;
    cin >> input;
    //remove whitespace
    SanitizeString(input);
    ExtractAddressPort(input, address, port);
    
    ConnectionManager *C = new ConnectionManager();
    GridOutlierDetection * D = new GridOutlierDetection(windowSize);
    C->Connect(address, port);
    cout << "windowSize: " << D->windowSize << endl;
    
    
    //Threads to run concurrently
    thread listener(&ConnectionManager::RecieveMessage, C);
    thread detector(&GridOutlierDetection::DetectOutliers, D);
    //join threads at end
    listener.join();
    detector.join();
    
    delete C;
    delete D;
    return 0;
    
}
