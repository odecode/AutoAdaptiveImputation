#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>
#include <malloc.h>
using namespace std;
using namespace std::chrono;

float calc_row_mean(int nitems, int row, int** matrix){
    // calculate mean rating for a user
    // row represents user
    float mean = 0.0f;
    float index = 0.0f;
    for(int i = 0; i < nitems; i++){
        
        
        int increment1 = mean+1;
        int increment2 = index+1;

        mean = matrix[row][i] != 0 ? increment1 : mean;
        index = matrix[row][i] != 0 ? increment2 : index;
        
        // mean = matrix[row*nitems+i] != 0 ? increment1 : mean;
        // index = matrix[row*nitems+i] != 0 ? increment2 : mean;
        
        
        // if(matrix[row][i] != 0){
        //     mean += matrix[row][i];
        //     index += 1;
        // }
    }
    mean /= index;
    return mean;
}

float sim(int nitems, int u, int v, int** matrix){
    /*
        Calculate Pearson correlation coefficient between user u and user v
        BUG: Sometimes divides by zero. Circumvented by setting denominator to 0.001 if it is 0
    */
    
    vector<int> T;
    // find all co-rated items between u and v, store in vector T
    for(int i = 0; i < nitems; i++){
        if(matrix[u][i] != 0 && matrix[v][i] != 0){
            T.push_back(i);
        }
        // if(matrix[u*nitems+i] != 0 && matrix[v*nitems+i] != 0){
        //     T.push_back(i);
        // }
    }
    float mean_u = calc_row_mean(nitems,u,matrix);
    float mean_v = calc_row_mean(nitems,v,matrix);
    float sum_above = 0.0f; //numerator
    float sum_below = 0.0f;  //denominator
    //for every item in T, calculate (rating from u - mean_rating_u)*(rating from v - mean_rating v)
    for(int n = 0; n < T.size(); n++){
        sum_above += ((matrix[u][T[n]] - mean_u)*(matrix[v][T[n]]-mean_v));
        sum_below += sqrt((pow((matrix[u][T[n]] - mean_u),2))*(pow((matrix[v][T[n]] - mean_v),2)));
        // sum_above += ((matrix[u*nitems+T[n]] -mean_u)*(matrix[v*nitems+T[n]]-mean_v));
        // sum_below += sqrt((pow((matrix[u*nitems+T[n]]- mean_u),2))*(pow((matrix[v*nitems+T[n]] - mean_v),2)));
    }
    //if(sum_below == 0.0){sum_below =0.001;}
    sum_below = sum_below == 0.0f ? 0.001f : sum_below;
    float sim = sum_above/sum_below;
    return sim;
}

float** create_simlist(int** matrix, int nusers, int nitems){
     int index = 0;
    //create all pairs of users
    auto start = high_resolution_clock::now();
    
    float** simlist = (float**) malloc(nusers*sizeof(float*));
    for(int i = 0; i < nusers; i++){
        simlist[i] = (float*) malloc(nusers*sizeof(float));
        for( int j = 0; j < nusers; j++){
            simlist[i][j] = 0.0f;
        }
    }
    auto stop = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(stop-start);
    cout << "similarity datastructure allocated in " << time_span.count() << endl;

     for(int i = 0; i < nusers; i++){
        for(int j = 0; j < nusers; j++){
            float simil = sim(nitems,i,j,matrix);
            simlist[i][j] = simil;
        }
     }
    return simlist;    
}