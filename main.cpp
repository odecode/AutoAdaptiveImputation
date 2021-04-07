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
using namespace std;
using namespace std::chrono;
const int nusers = 943; // number of users in matrix
const int nitems = 1682;  // number of items in matrix
const int lines = 100000;   //number of ratings in data set//



int** create_matrix(){
    /*
    Create the user-item rating matrix
    reads in lines from MovieLens data file
    append ratings into matrix
    return matrix
    */
    int** matrix = new int*[nusers];
    for(int i = 0; i<nusers; ++i){
        matrix[i] = new int[1682];
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
            linenum += 1;
        }
        else{break;}
    }
    return test_users;
}

float calc_row_mean(int row, int** matrix){
    // calculate mean rating for a user
    float mean = 0.0;
    int index = 0.0;
    for(int i = 0; i < nitems; i++){
        if(matrix[row][i] != 0){
            mean += matrix[row][i];
            index += 1;
        }
    }
    mean /= index;
    return mean;
}

float sim(int u, int v, int** matrix){
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
    }
    float mean_u = calc_row_mean(u,matrix);
    float mean_v = calc_row_mean(v,matrix);
    float sum_above = 0.0; //numerator
    float sum_below = 0.0;  //denominator
    //for every item in T, calculate (rating from u - mean_rating_u)*(rating from v - mean_rating v)
    for(int n = 0; n < T.size(); n++){
        sum_above += ((matrix[u][T[n]] - mean_u)*(matrix[v][T[n]]-mean_v));
        sum_below += sqrt((pow((matrix[u][T[n]] - mean_u),2))*(pow((matrix[v][T[n]] - mean_v),2)));
    }
    if(sum_below == 0.0){sum_below =0.001;}
    float sim = sum_above/sum_below;
    return sim;
}

map<pair<int,int>,float> create_simlist(int** matrix){
    /*
    similarity measures between all pairs of users.
    Store as (user1,user2): sim
    */
    pair<int, int> *pairs= new pair<int,int>[nusers*nusers];
    int index = 0;
    //create all pairs of users
    for(int i = 0; i < nusers; i++){
        for(int j = 0; j < nusers; j++){
            if(i != j && index < nusers*nusers-1){
                pair<int,int> PAIR;
                PAIR.first = i;
                PAIR.second = j;
                pairs[index] = PAIR;
                index++;
            }
        }
    }
    //calculate similarity between all pairs
    map<pair<int, int>, float> simlist;
    for(int i = 0; i < nusers*nusers; i++){
        pair<int,int> p = pairs[i];
        int user1 = p.first;
        int user2 = p.second;
        float simil = sim(user1,user2,matrix);
        simlist.insert(make_pair(p,simil));
    }
    delete pairs;
    return simlist;
}

vector<int> get_related_users(int user, int item, int** matrix, int test_users[]){
    /*
    Find all related users to target user
    Related users are all users who have rated target item
    TODO: last argument not used; need to refactor
    */
    vector<int> rel_users;
    for(int i = 0; i < nusers; i++){
        bool related = true;
        if(user != i){
            if(matrix[i][item] == 0){related = false;}
            if(related){rel_users.push_back(i);}
        }
    }
    return rel_users;
}

vector<int> get_related_items(int user, int item, int** matrix, vector<int> rel_use){
    /*
    Find all related items to target item
    Related items are items which target user and a related user has rated
    */
    
    set<int> rel_it;
    for(int i = 0; i < rel_use.size(); i++){
        for(int j = 0; j < nitems; j++){
            int user2 = rel_use[i];
            if(matrix[user2][j] != 0 && matrix[user][j] != 0){rel_it.insert(j);}
        } 
    }

    // turn set into vector for easier processing
    vector<int> related_items_vector;
    for(int i = 0; i < rel_it.size(); i++){
        auto j = *rel_it.begin();
        related_items_vector.push_back(j);
        rel_it.erase(rel_it.begin());
    }
    return related_items_vector;

}

vector<vector<int>> get_key_neighbors(int user, int item, int** matrix,int test_users[],int size){
    /*
    Find key neighbors
    Key neighbors are vectors of (user,item,rating) from related users to related items
    size parameter determines maximum size of key neighbors vector (to speed up computation)
    */
    vector<vector<int>> key_neighbors;
    vector<int> related_users = get_related_users(user,item,matrix,test_users);
    vector<int> related_items = get_related_items(user,item,matrix,related_users);

    for(int i = 0; i < related_users.size(); i++){
        if(key_neighbors.size() == size){break;}
        for(int j = 0; j < related_items.size(); j++){
            if(key_neighbors.size() == size){break;}
            vector<int> arr = {related_users[i],related_items[j],matrix[i][j]};
            key_neighbors.push_back(arr);
        }
    }
    return key_neighbors;
}

float** imputate_matrix(int user, int item, int** matrix,int test_users[], map<pair<int,int>,float> simlist){
    /*
    Matrix imputation function
    */
    
    // find key neighbors of target user and item
    vector<vector<int>> key_neigh = get_key_neighbors(user,item,matrix,test_users,nitems);
    float** impmatrix =new float*[nusers];
    for(int i = 0; i < nusers; i++){
        impmatrix[i] = new float[nitems];
        for(int j = 0; j < nitems; j++){
            impmatrix[i][j] = float(matrix[i][j]);
        }
    }
    for(int i = 0; i < key_neigh.size(); i++){
        vector<int> entry = key_neigh[i];
        int rating = entry[2];
        if(rating == 0){
            // find n (set to 20) key neighbors of current key neighbor of target user
            vector<vector<int>> key_neigh_knn = get_key_neighbors(entry[0],entry[1],matrix,test_users,20);
            int current_key_neighbor = entry[0];
            int current_item = entry[1];
            float mean_cur_key_neigh = calc_row_mean(current_key_neighbor,matrix);
            float sum_above = 0.0;
            float sum_below = 0.0;
            //loop through current key neighbors key neighbors
            //for each pair of neighbors, find similarity, calculate mean values, sum according to formula
            for(int j = 0; j < key_neigh_knn.size(); j++){
               vector<int> key_neigh_knn_entry = key_neigh_knn[j];
               int key_neighbors_neighbor = key_neigh_knn_entry[0];
               if(key_neighbors_neighbor != current_key_neighbor){
                   pair<int,int> p = make_pair(current_key_neighbor,key_neighbors_neighbor);
                   float sim = simlist.at(p);
                   float mean_key_neigh_neigh = calc_row_mean(key_neighbors_neighbor,matrix);
                  
                  sum_above += sim*((matrix[key_neighbors_neighbor][current_item]-mean_key_neigh_neigh));
                  sum_below += sim;

               }
            }
            if(sum_below == 0.0){sum_below = 0.001;}
            float imp_rating = mean_cur_key_neigh+(sum_above/sum_below);
            impmatrix[current_key_neighbor][current_item] = abs(imp_rating);
            //cout << "imputating " << imp_rating << " at " << current_key_neighbor << " " << current_item << endl;
        }      

    }
key_neigh.clear();
return impmatrix;
}

float predict_rating(int user,int item, int** matrix, int test_users[], map<pair<int,int>,float> simlist){
    /*
    Predicts rating from target user to target item
    Find Key neighbors, imputate matrix with artificial ratings, calculate predicted rating according to formula
    */
    
    int rating = matrix[user][item];
    if( rating != 0){
        cout << "Rating already observed" << endl;
        return float(rating);
    }
    else{
        float** imp_matrix = imputate_matrix(user,item,matrix,test_users,simlist);
        vector<vector<int>> P = get_key_neighbors(user,item,matrix,test_users,nitems);
        float sum_above = 0.0;
        float sum_below = 0.0;
        float mean_user = calc_row_mean(user,matrix);
        for(int i = 0; i < P.size(); i++){
            vector<int> entry = P[i];
            int neighbor = entry[0];
            pair<int,int> p = make_pair(user,neighbor);
            float sim = simlist.at(p);
            float mean_neighbor = calc_row_mean(neighbor,matrix);
            sum_above += sim*(imp_matrix[neighbor][item]-mean_neighbor);
            sum_below += sim;
        }
        float predicted_rating = mean_user+(sum_above/sum_below);
        if(predicted_rating > 5.0){predicted_rating = 5.0;}
        else if(predicted_rating < 0.0){predicted_rating = 0.0;}
        //P.clear();
        for(int i = 0; i < nusers; i++){
            delete[] imp_matrix[i];
        }
        delete[] imp_matrix;
        return predicted_rating;
    }

}

int main(){
    cout<< "hello world" << endl;
    int** rating_matrix = create_matrix();
    int** test_users = create_test_user_list(rating_matrix);
    auto start = high_resolution_clock::now();
    pair<int, int> *pairs = new pair<int,int>[888306];
    map<pair<int,int>,float> simlist = create_simlist(rating_matrix);
    auto stop = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(stop-start);
    cout << "simlist created in " << time_span.count() << " seconds"<< endl;
    int test_users_unique[nusers];
    for(int i=0;i<nusers;i++){test_users_unique[i]=i;}
    int index = 1;
    float MAE = 0.0;

    /*
    Calculate Mean Absolute Error of n (2000) predicted ratings 
    */
    while(index <= 2000){
       int user = test_users[index-1][0];
       int item = test_users[index-1][1];
       int real_rating = test_users[index-1][2];
       float pred = predict_rating(user,item,rating_matrix,test_users_unique,simlist);
       cout << "Prediction " << index << " real rating " << real_rating << " predicted rating " << pred << endl;
       float diff = abs(float(real_rating-pred));
       MAE += diff;
       index++; 
    }
    MAE = MAE/float(index);
    cout << "MAE " << MAE << endl;


}