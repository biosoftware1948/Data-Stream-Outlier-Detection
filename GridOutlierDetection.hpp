#ifndef GOD_H
#define GOD_H

#include "Globals.hpp"

using namespace std;

class GridOutlierDetection{
private:
    
    int currentPoints;
    int bins; //total bins needed
    const int MIN_GRID_VAL = -32768;
    const int MAX_GRID_VAL = 32767;
    int gridWidth;
    int tau;
    queue<int> binOrderIndex; // hold the index of point bins in order to easily remove from window
    void RemoveLastFromWindow(int &timestepToRemove);
    
public:
    int windowSize;
    vector<Bin> Bins; //hold all the bins
    vector< vector<AxisBin> > Grid;
    int dimensions;
    int partitions;
    
    GridOutlierDetection(int windowSize);
    void CreateGrid();
    dataPoint GetNextDataPoint();
    int AddToGrid(dataPoint dp); //return bin index in Bins to find easy
    void DetectOutliers();
};


#endif
