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
#include "ratingmatrices.h"
#include "related.h"
using namespace std;
using namespace std::chrono;
int allusers[943];
int allitems[1682];
int main(){
    cout<< "hello world" << endl;
    pair<int,int> usrxitm = assigconstants();
    nusers = usrxitm.first;
    nitems = usrxitm.second;
    for (int i = 0; i < nusers; i++)
    {
        allusers[i] = i;
    }
    for (int i = 0; i < nitems; i++)
    {
        allitems[i] = i;
    }
    int* pallusers = allusers;
    int* pallitems = allitems;

    
    
    int** rating_matrix = create_matrix();
    cout << "created matrix\n";
    int** test_users = create_test_user_list(rating_matrix);
    cout << "created test users\n";
    
    // auto start = high_resolution_clock::now();
    // //pair<int, int> *pairs = new pair<int,int>[888306];
    // //map<pair<int,int>,float> simlist = create_simlist(rating_matrix);
    // auto stop = high_resolution_clock::now();
    // duration<double> time_span = duration_cast<duration<double>>(stop-start);
    // cout << "simlist created in " << time_span.count() << " seconds"<< endl;
    
    auto start = high_resolution_clock::now();
    generateAllKeyNeighbors(rating_matrix,20,nusers,nitems,pallusers,pallitems);
    auto stop = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(stop-start);
    cout << "all key neighbors computed in in " << time_span.count() << endl;
    return 0;
}