#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <utility>
#include <chrono>
#include <malloc.h>
#include <bits/stdc++.h>
using namespace std;
#pragma once
int nusers; // number of users in matrix
int nitems;  // number of items in matrix
// int* pusers = &nusers;
// int* pitems = &nitems;
const int lines = 100000;   //number of ratings in data set//
int** matrix;
int** test_users;

pair<int,int> assigconstants(){
    ifstream myfile("u.data");
    string line;
    string row, col, strrating;
    int maxuser = 0;
    int maxitem = 0;
    while(getline(myfile, line)){
        
        stringstream ss(line);
        getline(ss,row,'\t');
        getline(ss,col,'\t');
        //getline(ss,strrating,'\t');
        int user = stoi(row); // convert string to integer
        int item = stoi(col);
        maxuser = user > maxuser ? user : maxuser;
        maxitem = item > maxitem ? item : maxitem;
        
    }
    myfile.close();
    pair<int,int> userxitem = make_pair(maxuser,maxitem);
    nusers = maxuser;
    nitems = maxitem;
    return userxitem;
}

int** create_matrix(){
    /*
    Create the user-item rating matrix
    reads in lines from MovieLens data file
    append ratings into matrix
    return matrix
    */
    // int** matrix = new int*[nusers];
    // for(int i = 0; i<nusers; ++i){
    //     matrix[i] = new int[1682];
    //     for(int j = 0; j < nitems; ++j){
    //         matrix[i][j] = 0;
    //     }
    // }

    //int* matrix = new int[nusers*nitems*sizeof(int)];
    matrix = (int**) malloc(nusers*sizeof(int*));
    int* m2 = (int*) malloc(nusers*nitems*sizeof(int));
    
    for(int i = 0; i < nusers; ++i){
        matrix[i] = m2 + i*nitems;
    }
    
    for(int i = 0; i < nusers; ++i){
        for(int j = 0; j < nitems; ++j){
            matrix[i][j] = 0;
        }
    }


    ifstream myfile("u.data");
    string line;
    string row, col, strrating;
    while(getline(myfile, line)){
        
        stringstream ss(line);
        getline(ss,row,'\t');
        getline(ss,col,'\t');
        getline(ss,strrating,'\t');
        int user = stoi(row); // convert string to integer
        int item = stoi(col);
        int rating = stoi(strrating);
        user--; // user numbers range from 1-943, while matrix indices range from 0-942. Subtract 1 from user to align with index
        item--; // same as user
        matrix[user][item] = rating;
        //matrix[user*nitems+item] = rating;

    }
    myfile.close();
    return matrix;
}

int** create_test_user_list(int** matrix){
    /*
    Set aside 20k ratings as test data,
    remove these ratings from rating matrix
    take these ratings from the data file, because they are randomly ordered
    */
    int** test_users = new int*[20000];
    for(int i = 0; i<20000; ++i){
        test_users[i] = new int[3];
        for(int j = 0; j < 3; ++j){
            test_users[i][j] = 0;
        }
    }

    //int* test_users = new int[nusers*3*sizeof(int)];
    const int amountOfTestUsers = 20000;
    const int dataItemsPerTestUser = 3;
    // test_users = (int**) malloc(amountOfTestUsers*sizeof(int*));
    // int* t2 = (int*) malloc(amountOfTestUsers*dataItemsPerTestUser*sizeof(int));
    // for(int i = 0; i < amountOfTestUsers; ++i){
    //     test_users[i] = t2 + i*dataItemsPerTestUser;
    // }
    
    // for(int i = 0; i < amountOfTestUsers; ++i){
    //     for(int j = 0; j < dataItemsPerTestUser; ++j){
    //         test_users[i][j] = 0;
    //     }
    // }
    ifstream myfile("u.data");
    string line;
    string row,col,strrating;
    int linenum = 0;
    while(getline(myfile,line)){
        if(linenum < lines*0.2){
            stringstream ss(line);
            getline(ss,row,'\t');
            getline(ss,col,'\t');
            getline(ss,strrating,'\t');
            int user = stoi(row);
            int item = stoi(col);
            int rating = stoi(strrating);
            user--;
            item--;
            test_users[linenum][0] = user;
            test_users[linenum][1] = item;
            test_users[linenum][2] = rating;
            matrix[user][item] = 0;
            // test_users[linenum*3+0] = user;
            // test_users[linenum*3+1] = item;
            // test_users[linenum*3+2] = rating;
            // matrix[user*nitems+item] = 0;
            
            linenum += 1;
        }
        else{break;}
    }
    return test_users;
}

bool compareUserEntries(int* entry1, int* entry2){
    return entry1[0] < entry2[0];
}

void sort_test_users_list(int** userlist){
    //sort(userlist,userlist+20000,compareUserEntries);
    for (int i = 0; i < 20000; i++)
    {
        for (int j = 0; j < 20000; j++)
        {
            int* temp = new int[3];
            bool leftltright = compareUserEntries(userlist[i],userlist[j]);
            if(leftltright){
                for (int k = 0; k < 3; k++)
                {
                    temp[k] = userlist[i][k];
                    userlist[i][k] = userlist[j][k];
                    userlist[j][k] = temp[k]; 
                }
            }
            else{
                for (int k = 0; k < 3; k++)
                {
                    temp[k] = userlist[j][k];
                    userlist[j][k] = userlist[i][k];
                    userlist[i][k] = temp[k];

                }
            }
            delete temp;
        }
        
    }
    
}