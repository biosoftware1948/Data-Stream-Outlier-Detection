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
#include <chrono>
using namespace std;
/*
 *Global variables declared here
 *INPUT_BUFFER is shared between listener and detector threads
 *Mutex is used for atomicity
 */
extern mutex bufferLock;
extern queue< vector<int> > INPUT_BUFFER;
extern bool complete;

/*
 *Helper functions
 */
/*
 *ExtractAddressPort takes the given input in form address:port and
 seperates them for easy use
 */
void ExtractAddressPort(string input, string &address, int& port);
/*
 *SanitizeString helps use withold whitespace free convention, it removes all
 whitespace
 */
void SanitizeString(string &s);
/*
 *dataPoint holds the timestep and dimensional data of inputs
 */
struct dataPoint{
    int timestep;
    vector<int> values; /*indexs are dimensions*/
};
/*
 *Axis bin, splits each axis into partitions, holds start and end of partition
 */
struct AxisBin{
    int start;
    int end;
};
/*
 *Bin, holds the axisBins representing the binspace, has a count and datapoints
 */
struct Bin{
    vector<int> AxisBinIndex; /*index 0: bin in dimensions 0 of grid*/
    int count;
    list<dataPoint> dataPoints;
    vector<int> neighbors; /*Holds the index of neighbor bins */
};
#endif
