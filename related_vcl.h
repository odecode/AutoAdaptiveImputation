
#include <immintrin.h>
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
#include "vectorclass.h"
#include "ratingmatrices.h"


using namespace std::chrono;
using namespace std;

struct relatedUsers{
    int *rel_users;
    int rel_users_size;
    int user;
};

relatedUsers get_related_users(int user, int item, int** matrix, int nusers, int nitems){
    //cout << "in get rel users 1\n";
    
    /*
    Find all related users to target user
    Related users are all users who have rated target item
    TODO: last argument not used; need to refactor
    */
    //int* rel_users_arr[nusers] = {0};
    int *rel_users_arr = (int*) malloc(nusers*sizeof(int));
    int veclen = 8;
    int leftOverFromArrayEnd = nusers % veclen;
    Vec8i minus1(-1);
    for (int i = 0; i < nusers-leftOverFromArrayEnd; i+=veclen)
    {
        minus1.store(&rel_users_arr[i]);
        
    }
    for (int i = nusers-leftOverFromArrayEnd; i < nusers; i++)
    {
        rel_users_arr[i] = -1;
    }
    
    int reluserssize = 0;
    // i == "user2"



    for (int user2 = 0; user2 < nusers - leftOverFromArrayEnd; user2 += veclen){

        Vec8i ratingdata, uservec;
        int ratingarr[veclen], userarr[veclen];
        for (int i = 0; i < veclen; i++)
        {
            ratingarr[i] = matrix[user2+i][item];
            userarr[i] = user2+i;
        }
        ratingdata.load(ratingarr);
        uservec.load(userarr);
        //related = matrix[i][*item] == 0 ? false : true;
        Vec8ib related = ratingdata != 0;
        uservec = select(related,uservec,-1);
        uservec.store(&rel_users_arr[user2]);
    
    }

    for (int user2 = nusers-leftOverFromArrayEnd; user2 < nusers; user2++)
    {
        bool related = true;
        if(user2 != user){
            related = matrix[user2][item] == 0 ? false : true;
        }

        if(related){
            rel_users_arr[user2] = user2;
        }
    }
    
    // for (int i = 0; i < nusers; i++)
    // {
    //     cout << rel_users_arr[i] << " ";
    // }
    // cout << endl;
    
 
    
    relatedUsers rel_users;
    rel_users.rel_users = rel_users_arr;
    rel_users.user = user;
    rel_users.rel_users_size = reluserssize;

    return rel_users;
}

struct relatedItems{
    int *rel_items;
    int item;
};

relatedItems get_related_items(int user, int item, int** matrix, relatedUsers rel_use, int nusers, int nitems){
    const int rel_items_size = nusers*nitems;
    int* rel_items = (int*) malloc(rel_items_size*sizeof(int));
    Vec8i minus1(-1);
    const int veclen = 8;
    int leftOverFromArrayEnd = (rel_items_size) % veclen;
    int leftOvernItems = nitems % veclen;
    for (int i = 0; i < rel_items_size-leftOverFromArrayEnd; i+=veclen)
    {
        minus1.store(&rel_items[i]);
    }
    
    for (int i = rel_items_size-leftOverFromArrayEnd; i < rel_items_size; i++)
    {
        rel_items[i] = -1;
    }

    for (int rel_user_index = 0; rel_user_index < nusers; rel_user_index++){
        int user2 = rel_use.rel_users[rel_user_index];
        if(user2 == -1) continue;
        for (int cur_item = 0; cur_item < nitems-leftOvernItems; cur_item+=veclen)
        {
            Vec8i user1vec(user);
            Vec8i user2vec(user2);
            Vec8i currentitemvec(cur_item,cur_item+1,cur_item+2,cur_item+3,cur_item+4,cur_item+5,cur_item+6,cur_item+7);
            
            Vec8i ratingvec_user1, ratingvec_user2;
            int user1ratings[veclen], user2ratings[veclen];
            for (int i = 0; i < veclen; i++)
            {
                user1ratings[i] = matrix[user][cur_item+i];
                user2ratings[i] = matrix[user2][cur_item+i];
            }
            ratingvec_user1.load(user1ratings);
            ratingvec_user2.load(user2ratings);
            /*
            uservec = select(related,uservec,-1);
            uservec.store(&rel_users_arr[user2]);
            */
            
            Vec8ib related = (ratingvec_user1 != 0) && (ratingvec_user2 != 0);
            currentitemvec = select(related,currentitemvec,-1);
            currentitemvec.store(&rel_items[cur_item]);

        } 
    }

    // cout << "related items\n\n";
    // for (int i = 0; i < rel_items_size; i++)
    // {

    //     if(rel_items[i] != -1) cout << rel_items[i] << " ";
    // }
    
    
    relatedItems rel_it;
    rel_it.rel_items = rel_items;
    rel_it.item = item;
    
    return rel_it;
}


struct keyNeighbors{
    struct relatedUsers rel_users;
    struct relatedItems rel_items;
    int user;
    int item;
};

keyNeighbors get_key_neighbors(int user, int item, int** matrix, int size, int nusers, int nitems){

    keyNeighbors keyneighbors;
    relatedUsers relUsers = get_related_users(user,item,matrix,nusers,nitems);
    relatedItems relItems = get_related_items(user,item,matrix,relUsers,nusers,nitems);

    keyneighbors.rel_users = relUsers;
    keyneighbors.rel_items = relItems;
    keyneighbors.user = user;
    keyneighbors.item = item;

    return keyneighbors;

}







