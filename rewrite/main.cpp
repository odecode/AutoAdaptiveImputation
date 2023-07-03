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
#include "related_vcl.h"
#include "simlist.h"

//#include "related.h"
using namespace std;
using namespace std::chrono;
// const int nusers = 943; // number of users in matrix
// const int nitems = 1682;  // number of items in matrix
// const int lines = 100000;   //number of ratings in data set//
int allusers[943];
int allitems[1682];


float** imputate_matrix(int user, int item, int** matrix, float** simlist){
    //cout << "in imp matrix\n";
    /*
    Matrix imputation function
    */
    
    // find key neighbors of target user and item
    //vector<vector<int>> key_neigh = get_key_neighbors(user,item,matrix,nitems,alluserrels);
    keyNeighbors key_neigh = get_key_neighbors(user,item,matrix,nusers,nusers,nitems);
    
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

    

    int key_neigh_size_users_size = key_neigh.rel_users.rel_users_size;
    int key_neigh_size_items_size = key_neigh.rel_items.rel_items_size;
    // int key_neigh_size_users_size = key_neigh.rel_users.rel_users_size;
    // int key_neigh_size_items_size = key_neigh.rel_items.rel_items_size;
    //#pragma omp parallel for
    

    Node* current_user = key_neigh.rel_users.rel_users;
    Node* current_item = key_neigh.rel_items.rel_items;

    int currentuser_data = current_user->data;
    int currentitem_data = current_item->data;

    while(current_user->next != NULL && currentuser_data < nusers && currentuser_data >= 0){
        while(current_item->next != NULL && currentitem_data < nitems && currentitem_data >= 0){
            //cout << currentuser_data << " " << currentitem_data << endl;
            int rating = matrix[currentuser_data][currentitem_data];
            if(rating == 0){
                float mean_cur_key_neigh = calc_row_mean(nitems,currentuser_data,matrix);
                float sum_above = 0.0f;
                float sum_below = 0.0f;
                keyNeighbors key_neigh_knn = get_key_neighbors(currentuser_data,currentitem_data,matrix,20,nusers,nitems);
                //int current_key_neighbor = entry[0];
                //int current_item = entry[1];
                
                
                
                

                Node* key_neighbors_neighbor = key_neigh_knn.rel_users.rel_users;
                int key_neigh_neigh = key_neighbors_neighbor->data;
                while(key_neighbors_neighbor->next != NULL){
                    if(key_neigh_neigh != currentuser_data){
                        // pair<int,int> p = make_pair(currentuser_data,key_neigh_neigh);
                        // float sim = simlist.at(p);
                        float sim = simlist[currentuser_data][key_neigh_neigh];
                        float mean_key_neigh_neigh = calc_row_mean(nitems,key_neigh_neigh,matrix);
                  
                        sum_above += sim*((matrix[key_neigh_neigh][currentitem_data]-mean_key_neigh_neigh));
                        sum_below += sim;
                        
                        key_neighbors_neighbor = key_neighbors_neighbor->next;
                        key_neigh_neigh = key_neighbors_neighbor->data;
                    }
                }
            if(sum_below == 0.0f){sum_below = 0.001f;}
            float imp_rating = mean_cur_key_neigh+(sum_above/sum_below);
            impmatrix[currentuser_data][currentitem_data] = abs(imp_rating);
            } // end if rating == 0
            current_item = current_item->next;
            currentitem_data = current_item->data;
        }
        current_user = current_user->next;
        currentuser_data = current_user->data;
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
        float** imp_matrix = imputate_matrix(user,item,matrix,simlist);
        keyNeighbors P = get_key_neighbors(user,item,matrix,20,nusers,nitems);
        //int pneighsize = P.rel_users.rel_users_size;
        Node* neighbor = P.rel_users.rel_users;
        int neighbordata = neighbor->data;
        
        int pneighsize = P.rel_users.rel_users_size;
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
            neighbor = neighbor->next;
            neighbordata = neighbor->data;
        }
        float predicted_rating = mean_user+(sum_above/sum_below);
        // if(predicted_rating > 5.0){predicted_rating = 5.0;}
        // else if(predicted_rating < 0.0){predicted_rating = 0.0;}
        predicted_rating = predicted_rating > 5.0f ? 5.0f : predicted_rating;
        predicted_rating = predicted_rating < 0.0f ? 0.0f : predicted_rating;

        free(imp_matrix[0]);
        free(imp_matrix);
        
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