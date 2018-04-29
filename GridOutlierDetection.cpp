#include "GridOutlierDetection.hpp"

GridOutlierDetection::GridOutlierDetection(int windowSize){
    this->windowSize = windowSize;
}

dataPoint GridOutlierDetection::GetNextDataPoint(){
    /*Wait for item to enter buffer*/
    while(INPUT_BUFFER.size() < 1){
        if(complete){
            exit(0);
        }
    }
    /*Mutex grab then convert buffer info to dataPoint */
    bufferLock.lock();
    vector<int> v = INPUT_BUFFER.front();
    INPUT_BUFFER.pop();
    bufferLock.unlock();
    dataPoint dp;
    if (v.size() == 0){
        dp.timestep = -10;
        return dp;
    }
    if(v[0] < -1){
        dp.timestep = -10;
        return dp;
    }
    dp.timestep = v[0];
    
    if(dp.timestep == -1){
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
    
    this->bins = pow(this->partitions, this->dimensions);

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
            else{
                break;
            }

        }
        if( dim_count == this->dimensions){
            Bins[i].count++;
            Bins[i].dataPoints.push_back(dp);
            return i;
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
        
        //Add neighbors to bin, also add this bin to neighbors list
        for(int n = 0; n < this->Bins.size(); ++n){
            int all_dim = 0;
            for(int d = 0; d < this->Bins[n].AxisBinIndex.size(); ++d){
                if ((this->Grid[d][this->Bins[n].AxisBinIndex[d]].start -1 == this->Grid[d][b.AxisBinIndex[d]].end) || (this->Grid[d][this->Bins[n].AxisBinIndex[d]].end +1 == this->Grid[d][b.AxisBinIndex[d]].start) || (this->Grid[d][this->Bins[n].AxisBinIndex[d]].start == this->Grid[d][b.AxisBinIndex[d]].end) || (this->Grid[d][this->Bins[n].AxisBinIndex[d]].start  == this->Grid[d][b.AxisBinIndex[d]].start)){
                    all_dim++;
                }
                else{
                    break;
                }

            }//test dis
            if(all_dim == this->Bins[n].AxisBinIndex.size()){
                //Add neighbors to new bin
                if(find(b.neighbors.begin(), b.neighbors.end(), n) == b.neighbors.end()){
                    b.neighbors.push_back(n);
                }
                
                //Add new bin to neighbors of other bins
                 if(find(this->Bins[n].neighbors.begin(), this->Bins[n].neighbors.end(), 1) == this->Bins[n].neighbors.end()){
                 this->Bins[n].neighbors.push_back(this->Bins.size());
                 }
            }
        }
        
        b.count = 1;
        b.dataPoints.push_back(dp);
        this->Bins.push_back(b);
        return Bins.size()-1;
    }
}

void GridOutlierDetection::RemoveLastFromWindow(){
    /*Pop from bin queue*/
    int binIndex = this->binOrderIndex.front();
    this->binOrderIndex.pop();
    this->Bins[binIndex].dataPoints.pop_front();
    this->Bins[binIndex].count--;
    this->currentPoints--;
}

void GridOutlierDetection::DetectOutliers(){
    int currentBinIndex = 0;
    this->CreateGrid();
    bool outlier = true;
    
    /*First fill the window with data*/
    dataPoint dp;
    while(currentPoints < this->windowSize){
        dp = this->GetNextDataPoint();
        if(dp.timestep == -10){
            continue;
        }
        currentBinIndex = this->AddToBin(dp);
        /*push bin index to queue so we can find last point in window*/
        this->binOrderIndex.push(currentBinIndex);
    }
    /*Window full, start detecting outliers. Let's go boys! */
    while(1){
        dp = this->GetNextDataPoint();
        if(dp.timestep == -10){
            continue;
        }
        currentBinIndex = this->AddToBin(dp);
        /*push bin index to queue so we can find last point in window*/
        this->binOrderIndex.push(currentBinIndex);
        outlier = true;
        for(int i = 0; i < Bins[currentBinIndex].neighbors.size(); ++i){
            if (Bins[Bins[currentBinIndex].neighbors[i]].count >= this->tau){
                outlier = false;
                break;
            }
        }
        if(Bins[currentBinIndex].count > this->tau){
            outlier = false;
        }
        if(outlier){
            cout << "Outlier at Timestep: " << dp.timestep << " With values: ";
            for(int i = 0; i < this->dimensions; ++i){
                cout << dp.values[i];
                if(i != this->dimensions -1){
                    cout << ", ";
                }
            }
            cout << endl;
        }
        this->RemoveLastFromWindow();
    }
    return;
}
