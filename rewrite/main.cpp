#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <utility>
#include <chrono>
#include <set>
#include <malloc.h>
#include <omp.h>
#include "users.h"
#include "ratingmatrices.h"
#include "newrelated_vcl.h"
#include "simlist.h"

//#include "related.h"
using namespace std;
using namespace std::chrono;
// const int nusers = 943; // number of users in matrix
// const int nitems = 1682;  // number of items in matrix
// const int lines = 100000;   //number of ratings in data set//
int allusers[943];
int allitems[1682];


float** imputate_matrix(int user, int item, int** matrix, float** simlist, int* rel_users_arr, int* rel_items){
    //cout << "in imp matrix\n";
    /*
    Matrix imputation function
    */
    
    // find key neighbors of target user and item
    //vector<vector<int>> key_neigh = get_key_neighbors(user,item,matrix,nitems,alluserrels);
    keyNeighbors key_neigh = get_key_neighbors(user,item,matrix,nusers,nusers,nitems,rel_users_arr,rel_items);
    
    // float** impmatrix =new float*[nusers];
    // for(int i = 0; i < nusers; i++){
    //     impmatrix[i] = new float[nitems];
    //     for(int j = 0; j < nitems; j++){
    //         impmatrix[i][j] = float(matrix[i][j]);
    //     }
    // }

    float** impmatrix = (float**) malloc(nusers*sizeof(float*));
    float* imp2 = (float*) malloc(nusers*nitems*sizeof(float));
    
    for(int i = 0; i < nusers; ++i){
        impmatrix[i] = imp2 + i*nitems;
    }

    for(int i = 0; i < nusers; ++i){
        for(int j = 0; j < nitems; ++j){
            impmatrix[i][j] = (float) matrix[i][j];
        }
    }

    

    int key_neigh_size_users_size = key_neigh.rel_users.size();
    int key_neigh_size_items_size = key_neigh.rel_items.size();
    // int key_neigh_size_users_size = key_neigh.rel_users.rel_users_size;
    // int key_neigh_size_items_size = key_neigh.rel_items.rel_items_size;
    //#pragma omp parallel for
    

    // Node* current_user = key_neigh.rel_users.rel_users;
    // Node* current_item = key_neigh.rel_items.rel_items;

    // int currentuser_data = current_user->data;
    // int currentitem_data = current_item->data;
    
    int rel_use_size = key_neigh.rel_users.size();
    int rel_it_size = key_neigh.rel_items.size();
    int current_user = -1;
    int current_item = -1;

    if(rel_use_size > 0) int current_user = key_neigh.rel_users[0];
    if(rel_it_size > 0) int current_item = key_neigh.rel_items[0];
    
    // int current_item = key_neigh.rel_items[0];
    int curuserindex = 0;
    int curitemindex = 0;
    // cout << curuserindex << " " << current_user << " " << current_item << endl;
    while(curuserindex < rel_use_size && current_user < nusers && current_user >= 0){
        // cout << "curuserindex " << curuserindex << endl;
        while(curitemindex < rel_it_size && current_item < nitems && current_item >= 0){
            int rating = matrix[current_user][current_item];
            if(rating == 0){
                float mean_cur_key_neigh = calc_row_mean(nitems,current_user,matrix);
                float sum_above = 0.0f;
                float sum_below = 0.0f;
                keyNeighbors key_neigh_knn = get_key_neighbors(current_user,current_item,matrix,20,nusers,nitems,rel_users_arr,rel_items);
                std::vector<int> key_neighbors_neighbors = key_neigh_knn.rel_users;
                int key_neigh_neigh = key_neighbors_neighbors[0];
                for(int x = 0; x < key_neighbors_neighbors.size(); x++){
                    if(key_neigh_neigh != current_user){
                        float sim = simlist[current_user][key_neigh_neigh];
                        float mean_key_neigh_neigh = calc_row_mean(nitems,key_neigh_neigh,matrix);
                  
                        sum_above += sim*((matrix[key_neigh_neigh][current_item]-mean_key_neigh_neigh));
                        sum_below += sim;
                        
                        key_neigh_neigh = key_neighbors_neighbors[x];
                    }
                }
            if(sum_below == 0.0f){sum_below = 0.001f;}
            float imp_rating = mean_cur_key_neigh+(sum_above/sum_below);
            impmatrix[current_user][current_item] = abs(imp_rating);
            
            }
            curitemindex++;
        }
        curuserindex++;
    }
    
return impmatrix;
}

float predict_rating(int user,int item, int** matrix, float** simlist){
    /*
    Predicts rating from target user to target item
    Find Key neighbors, imputate matrix with artificial ratings, calculate predicted rating according to formula
    */
    //cout << "in predict rating\n";
    int rating = matrix[user][item];
    //int rating = matrix[user*nitems+item];
    if( rating != 0){
        cout << "Rating already observed" << endl;
        return -1.0f;
    }
    else{
        int rel_items_size = nusers*nitems;
        int* rel_items = (int*) malloc(rel_items_size*sizeof(int));
        int *rel_users_arr = (int*) malloc(nusers*sizeof(int));
        float** imp_matrix = imputate_matrix(user,item,matrix,simlist,rel_users_arr,rel_items);
        keyNeighbors P = get_key_neighbors(user,item,matrix,20,nusers,nitems,rel_users_arr,rel_items);
        //int pneighsize = P.rel_users.rel_users_size;
        // Node* neighbor = P.rel_users.rel_users;
        // int neighbordata = neighbor->data;
        std::vector<int> neighbors = P.rel_users;
        int neighbordata = neighbors[0];

        const int pneighsize = P.rel_users.size();
        float sum_above = 0.0f;
        float sum_below = 0.0f;
        float mean_user = calc_row_mean(nitems,user,matrix);
        for(int i = 0; i < pneighsize; i++){
            //vector<int> entry = P[i];
            // pair<int,int> p = make_pair(user,neighbordata);
            float sim = simlist[user][neighbordata];
            float mean_neighbor = calc_row_mean(nitems,neighbordata,matrix);
            sum_above += sim*(imp_matrix[neighbordata][item]-mean_neighbor);
            //sum_above += sim*(imp_matrix[neighbor*nitems+item]-mean_neighbor);
            sum_below += sim;
            // neighbor = neighbor->next;
            neighbordata = neighbors[i];
        }
        float predicted_rating = mean_user+(sum_above/sum_below);
        // if(predicted_rating > 5.0){predicted_rating = 5.0;}
        // else if(predicted_rating < 0.0){predicted_rating = 0.0;}
        predicted_rating = predicted_rating > 5.0f ? 5.0f : predicted_rating;
        predicted_rating = predicted_rating < 0.0f ? 0.0f : predicted_rating;

        free(imp_matrix[0]);
        free(imp_matrix);
        free(rel_items);
        free(rel_users_arr);
        
        return predicted_rating;
    }

}





int main(int argc, char** argv){
    cout<< "hello world" << endl;
    pair<int,int> usersxitems = assigconstants();
    int** rating_matrix = create_matrix();
    cout << "created matrix\n";
    int** test_users = create_test_user_list(rating_matrix);
    cout << "created test users\n";

    for (int i = 0; i < nusers; i++)
    {
        allusers[i] = i;
    }

    for (int i = 0; i < nitems; i++)
    {
        allitems[i] = i;
    }
    
    
    auto start = high_resolution_clock::now();
    float** simlist = create_simlist(rating_matrix,nusers,nitems);
    auto stop = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(stop-start);
    cout << "simlist created in " << time_span.count() << " seconds"<< endl;
    
    start = high_resolution_clock::now();
    sort_test_users_list(test_users);
    stop = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(stop-start);
    cout << "Test users sorted in " << time_span.count() << endl;
    
    
    // cout << "sorting test users\n";
    // start = high_resolution_clock::now();
    // sorttestusers(test_users,0,20000-1);   
    // stop = high_resolution_clock::now();
    // time_span = duration_cast<duration<double>>(stop-start);
    // cout << "test users sorted in " << time_span.count() << endl;
    
    int index = 1;
    float MAE = 0.0f;
    int maxloops = 50;
    if(argc > 1){
        maxloops =atoi(argv[1]);
    }
    /*
    Calculate Mean Absolute Error of n (2000) predicted ratings 
    */
    duration<double> total_time = duration_cast<duration<double>>(stop-stop);
   
    while(index <= maxloops){
       int user = test_users[index-1][0];
       int item = test_users[index-1][1];
       int real_rating = test_users[index-1][2];
       user = allusers[user];
       int* puser = &user;
       int* pitem = &item;
       start = high_resolution_clock::now();
       float pred = predict_rating(user,item,rating_matrix,simlist);
       stop = high_resolution_clock::now();
       time_span = duration_cast<duration<double>>(stop-start);
       total_time += time_span;
       cout << "-" << index << ": real rating " << real_rating << " predicted rating " << pred << " Time "<< time_span.count() << endl;
       float diff = abs(float(real_rating-pred));
       MAE += diff;
       index++; 
    }
    MAE = MAE/float(index);
    cout << "MAE " << MAE << endl;
    cout << "Total time " << total_time.count() << endl;
}