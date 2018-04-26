#ifndef GOD_H
#define GOD_H

#include "Globals.hpp"

using namespace std;

class GridOutlierDetection{
private:
    /*
     *Holds the amount of points currently in the window
     */
    int currentPoints;
    /*
     *Holds the total bins needed, calculated as partitions^dimensions
     */
    int bins;
    /*
     *Minimum possible 16 bit signed input value
     */
    const int MIN_GRID_VAL = -32768;
    /*
     *Maximum possible 16 bit signed input value
     */
    const int MAX_GRID_VAL = 32767;
    /*
     *Any bin with a value less than tau contains outliers
     *calculated as ceil(log10(partitions))
     */
    int tau;
    /*
     *Holds the order of bins that new datapoints fall into, this way when we remove the last point in the
     window we can take the bin in the front of the queue and remove the point from it. Everytime a new
     points comes in we push its bin to the queue
     */
    queue<int> binOrderIndex;
    /*
     *Removes the last point in the sliding window
     *PARAM: timestepToRemove is the last timestep in the window, in the case of a missed message it will
     jump to the next timestep
     */
    void RemoveLastFromWindow(int &timestepToRemove);
    
public:
    /*
     *Holds the window size used in the sliding window, set by constructor
     */
    int windowSize;
    /*
     *Holds all the bins, bins are created dynamically as new points come in to save memory
     */
    vector<Bin> Bins;
    /*This is the partitional dimension grid, each row is a dimensions and each column is the partition of that dimension
     */
    vector< vector<AxisBin> > Grid;
    /*
     *The total dimensions of incoming data
     */
    int dimensions;
    /*
     *How many bins each axis is partitioned into calculated as partitions^(dimensions+1) = windowSize
     */
    int partitions;
    /*
     *Constructor
     *PARAM: windowSize sets windowSize as specified by client
     */
    GridOutlierDetection(int windowSize);
    /*
     *Creates the grid based on dimensionality of incoming data and the window size
     */
    void CreateGrid();
    /*
     *Gets the next data points from the buffer and puts it into dataPoint struct
     *
     *ALTERS GLOBAL VARIABLE INPUT_BUFFER, grabs a mutex before hand for atomicity
     */
    dataPoint GetNextDataPoint();
    /*
     *Adds the new datapoint to a bin. If no suitable bin exists it creates a new one
     *PARAM: dp, the new datapoint to add
     *RETURNS: index of the new bin so it can be looked up in Bins vector quickly
     */
    int AddToBin(dataPoint dp);
    /*
     *Function detects outliers, easy wrapper for this class that allows a thread to run it concurrently
     while listening for new data.
     *Function runs untill the listener thread returns
     */
    void DetectOutliers();
};


#endif
