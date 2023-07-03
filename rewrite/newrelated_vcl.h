
#include <string>
#include <vector>
#include <cmath>
#include <malloc.h>
#include <omp.h>
#include "vectorclass.h"
#include "ratingmatrices.h"

struct keyNeighbors{
    std::vector<int> rel_users;
    std::vector<int> rel_items;
    int user;
    int item;
};

std::vector<int> get_related_users(int user, int item, int** matrix, int nusers, int nitems, int* rel_users_arr){
    // int *rel_users_arr = (int*) malloc(nusers*sizeof(int));
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
    std::vector<int> rel_users;
    int reluser_index = 0;
    rel_users.reserve(nusers);
    for(int i = 0; i < nusers; i++){
        if(rel_users_arr[i] == -1) continue;
        else{
            rel_users.push_back(rel_users_arr[i]);
            reluser_index++;
            reluserssize++;
        }
    }
    rel_users.resize(reluser_index);
    // free(rel_users_arr);
    return rel_users;
}

std::vector<int> get_related_items(int user, int item, int** matrix, std::vector<int> rel_use, int nusers, int nitems, int* rel_items){
    const int rel_items_size = nusers*nitems;
    // int* rel_items = (int*) malloc(rel_items_size*sizeof(int));
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

    for(int user2 = 0; user2 < rel_use.size(); user2++){
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
                user2ratings[i] = matrix[i][cur_item+i];
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

    std::vector<int> rel_it;
    rel_it.reserve(nitems);
    int actual_rel_items_size = 0;
    for(int i = 0; i < nusers; i++){
        if(rel_items[i] == -1) continue;
        else{
            rel_it.push_back(rel_items[i]);
            ++actual_rel_items_size;
        }
    }
    rel_it.resize(actual_rel_items_size);
    // free(rel_items);
    return rel_it;
}

keyNeighbors get_key_neighbors(int user, int item, int** matrix, int size, int nusers, int nitems, int* rel_users_arr, int* rel_items_arr){

    keyNeighbors keyneighbors;
    std::vector<int> relUsers = get_related_users(user,item,matrix,nusers,nitems,rel_users_arr);
    std::vector<int> relItems = get_related_items(user,item,matrix,relUsers,nusers,nitems,rel_items_arr);

    keyneighbors.rel_users = relUsers;
    keyneighbors.rel_items = relItems;
    keyneighbors.user = user;
    keyneighbors.item = item;

    return keyneighbors;

}