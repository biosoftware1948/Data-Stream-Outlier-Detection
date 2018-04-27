#include "GridOutlierDetection.hpp"

GridOutlierDetection::GridOutlierDetection(int windowSize){
    this->windowSize = windowSize;
}

dataPoint GridOutlierDetection::GetNextDataPoint(){
    /*Wait for item to enter buffer*/
    while(INPUT_BUFFER.size() < 1){
        ;
    }
    /*Mutex grab then convert buffer info to dataPoint */
    lock_guard<mutex> guard(bufferLock);
    vector<int> v = INPUT_BUFFER.front();
    INPUT_BUFFER.pop();
    dataPoint dp;
    dp.timestep = v[0];
    if(dp.timestep == -1){
        cout << "Exit message detected, Client has disconnected, detector will now exit" << endl;
        exit(0);
    }
    for(int i = 1; i <v.size(); ++i){
        dp.values.push_back(v[i]);
    }
    
    return dp;
}

void GridOutlierDetection::CreateGrid(){
    /*Get point and set information about data */
    dataPoint dp = this->GetNextDataPoint();
    this->dimensions = dp.values.size();//first dim is timestep
    this->partitions = round(pow(this->windowSize, (float(1)/(float(this->dimensions+1))))); //p^d+1 = w
    this-> tau = ceil(log10(this->partitions));
    cout << "Tau: " << this->tau << endl;
    cout << "partions: " << this->partitions << endl;
    
    this->bins = pow(this->partitions, this->dimensions);
    cout << "dimensions: " << this->dimensions << endl;
    cout << "Bins: " << this->bins << endl;

    /* Create grid */
    for(int i = 0; i < this->dimensions; ++i){
        int partition_size = round( float(pow(2, 16)) / float(this->partitions) );
        int start = MIN_GRID_VAL;
        int end = MIN_GRID_VAL + partition_size;
        vector<AxisBin> axis;
        /*Partition dimensions */
        for( int j = 0; j < this->partitions; ++j){
            AxisBin b;
            b.start = start;
            b.end = end;
            
            axis.push_back(b);
            start = end + 1;
            end = start + partition_size;
        }
        this->Grid.push_back(axis);
        
    }
    /*Add first point that we used to create grid into bin */
    this->AddToBin(dp);
}


int GridOutlierDetection::AddToBin(dataPoint dp){
    this->currentPoints++;
    bool binExists = false;
    int binIndex;
    
    /*Search for existence of bin */
    for (int i = 0; i < this->Bins.size(); ++i){
        int dim_count = 0;
        for(int d = 0; d < this->dimensions; ++d){
            if((dp.values[d] >= this->Grid[d][Bins[i].AxisBinIndex[d]].start) && (dp.values[d] <= this->Grid[d][Bins[i].AxisBinIndex[d]].end)){
                ++dim_count;
            }
            if( dim_count == this->dimensions){
                binIndex = i;
                Bins[binIndex].count++;
                Bins[binIndex].dataPoints.push_back(dp);
                return binIndex;
            }
        }
    }
    /*Bin doesnt exist so create new one */
    if(!binExists){
        Bin b;
        b.AxisBinIndex = vector<int>(this->dimensions, 0);
        for(int d = 0; d < this->dimensions; ++d){
            for(int p = 0; p < this->partitions; ++p){
                if( (dp.values[d] >= this->Grid[d][p].start) && (dp.values[d] <=  this->Grid[d][p].end)){
                    b.AxisBinIndex[d] = p;
                }
            }
        }
        b.count = 1;
        b.dataPoints.push_back(dp);
        this->Bins.push_back(b);
        return Bins.size()-1;
    }
}

void GridOutlierDetection::RemoveLastFromWindow(int &t){
    /*Pop from bin queue*/
    int binIndex = this->binOrderIndex.front();
    this->binOrderIndex.pop();
    bool deleted = false;
    /*Sometimes inputs are missed by listener, if this is the case we jump to next timestep with while loop */
    while(!deleted){
        for(auto it = this->Bins[binIndex].dataPoints.begin(); it != this->Bins[binIndex].dataPoints.end(); ++it){
            if(it->timestep == t){
                this->Bins[binIndex].dataPoints.erase(it);
                this->Bins[binIndex].count--;
                --this->currentPoints;
                return;
            }
        }
        /*Input doesnt exist, jump to next timestep*/
        ++t;
    }
}

void GridOutlierDetection::DetectOutliers(){
    int timestepToRemove = 1;
    int currentBinIndex = 0;
    this->CreateGrid();
    
    /*First fill the window with data*/
    dataPoint dp;
    while(currentPoints < this->windowSize){
        dp = this->GetNextDataPoint();
        currentBinIndex = this->AddToBin(dp);
        /*push bin index to queue so we can find last point in window*/
        this->binOrderIndex.push(currentBinIndex);
    }
    /*Window full, start detecting outliers. Let's go boys! */
    while(1){
        dp = this->GetNextDataPoint();
        currentBinIndex = this->AddToBin(dp);
        /*push bin index to queue so we can find last point in window*/
        this->binOrderIndex.push(currentBinIndex);
        if (Bins[currentBinIndex].count <= this->tau){
            cout << "Outlier at Timestep: " << dp.timestep << " With values: ";
            for(int i = 0; i < this->dimensions; ++i){
                cout << dp.values[i];
                if(i != this->dimensions -1){
                    cout << ", ";
                }
            }
            cout << endl;
        }
        this->RemoveLastFromWindow(timestepToRemove);
        ++timestepToRemove;
    }
    return;
}
