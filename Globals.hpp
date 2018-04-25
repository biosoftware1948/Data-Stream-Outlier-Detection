#ifndef globals_h
#define globals_h

#include<iostream>
#include<stdio.h>
#include<string>
#include <string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include <cassert>
#include <algorithm>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <sstream>
#include <cmath>
#include <list>
#include <mutex>

using namespace std;

extern mutex bufferLock;
extern queue< vector<int> > INPUT_BUFFER;

void ExtractAddressPort(string input, string &address, int& port);
void SanitizeString(string &s);

struct dataPoint{
    int timestep;
    vector<int> values; //indexs are dimensions
};

struct AxisBin{
    int start; //index are dimensions
    int end;
};

struct Bin{
    vector<int> AxisBinIndex; //index 0: bin in dimensions 0 of grid,
    int count;
    int tau;
    list<dataPoint> dataPoints;
    bool containsOutliers;
};
#endif
