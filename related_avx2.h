
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
#include "ratingmatrices.h"
using namespace std;
using namespace std::chrono;
using namespace std;

struct relatedUsers{
    int32_t *rel_users;
    int rel_users_size;
    int user;
};

relatedUsers get_related_users(int user, int item, int** matrix, int nusers, int nitems){
    
    /*
    Find all related users to target user
    Related users are all users who have rated target item

    Returns 

    */
    int32_t *rel_users_arr = (int32_t*) malloc(nusers*sizeof(int32_t));
    int veclen = 8;
    int leftOverFromArrayEnd = nusers % veclen;
    const int32_t negone = -1;
    const int32_t one = 1; 
    __m256i minus1 = _mm256_set_epi32(negone,negone,negone,negone,negone,negone,negone,negone);
    __m256i maskones = _mm256_set_epi32(one,one,one,one,one,one,one,one);

    
   for (int i = 0; i < veclen; i++)
   {
       //zeromask[i] = 0;
     // cout << minus1[i] << " ";
   }
   cout << endl;

    for (int i = 0; i < nusers-leftOverFromArrayEnd; i+=veclen)
    {
        // cout << rel_users_arr[i] << " " << rel_users_arr[i+1] << " " << rel_users_arr[i+2] << " " << rel_users_arr[i+3] << " " << rel_users_arr[i+4] << " " << rel_users_arr[i+5] << " " << rel_users_arr[i+6] << " " << rel_users_arr[i+7] << " ";
        // cout << endl;
        _mm256_maskstore_epi32(&rel_users_arr[i],minus1,minus1);
        //_mm256_store_si256(rel_users_arr[i],minus1);
        // cout << rel_users_arr[i] << " " << rel_users_arr[i+1] << " " << rel_users_arr[i+2] << " " << rel_users_arr[i+3] << " " << rel_users_arr[i+4] << " " << rel_users_arr[i+5] << " " << rel_users_arr[i+6] << " " << rel_users_arr[i+7] << " ";
        // cout << endl << endl;
        //rel_users_arr[i] = -1; //the original code that we vectorize
    }

    // store -1 into the last part of array, which didnt fit into vectors
    for (int i = nusers-leftOverFromArrayEnd; i < nusers; i++)
    {
        rel_users_arr[i] = (int32_t) -1;
    }
    


    int reluserssize = 0;
    
    /* We use 32 bit int SIMD with 256-bit vector registers
        256/32 = 8 --> we can fit 8 ints at a time
        To loop through the array rel_users_arr we take modulo 8 to find out division remainder
        and loop through the even number with SIMD, and do the remainder without simd

        UPDATE: Apparently it compiles to 64-bit ints (at least when I debug the cmpeq instruction) so we change veclen to 4
        since then we can fit 256/64 = 4 
    */
   const int32_t zero = 0;
   __m256i zeromask = _mm256_set1_epi64x(zero);
// //   __m256i mask;
//    for (int i = 0; i < veclen; i++)
//    {
//        //zeromask[i] = 0;
//       cout << zeromask[i] << " ";
//    }
   cout << endl;
   
    for (int user2 = 0; user2 < nusers - leftOverFromArrayEnd; user2 += veclen){
        __m256i ratingdata;
        __m256i uservec;
        
        // load a vector of ratings from matrix (from user2 to user2 + 8)
        // create vector of users
        cout << "rating data " << endl;
        for (int i = 0; i < veclen; i++)
        {
            ratingdata[i] = (int32_t) matrix[user2+i][item];
            uservec[i] = (int32_t) user2+i;
            cout << ratingdata[i] << " ";
        }
        cout << endl;
        
        
        /* Compare the ratings with mask of 0's to get a boolean mask telling if users are related or not */ 
        __m256i related = _mm256_cmpeq_epi32(ratingdata,zeromask);
        cout << "related " << endl;
       for (int i = 0; i < 8; i++)
       {
           cout << related[i] << " ";
       }
       cout << endl;
        
    cout  << "before maskstore" << endl;
        for (int i = 0; i < 8; i++)
    {
        cout << rel_users_arr[user2+i] << " ";

    }
    cout << endl;
        _mm256_maskstore_epi32(&rel_users_arr[user2],related,uservec);
    cout  << "after maskstore" << endl;
        for (int i = 0; i < 8; i++)
    {
        cout << rel_users_arr[user2+i] << " ";

    }
    cout << endl;

    }
    for (int user2 = nusers-leftOverFromArrayEnd; user2 < nusers; user2++)
    {
        bool related = true;
        if(user != user2){
        //cout << "in get rel users 3\n";
            related = matrix[user2][item] == 0 ? false : true;
            if(related){
                rel_users_arr[user2]=(int32_t) user2;
            }
        }
    }

    for (int i = 0; i < nusers; i++)
    {
        cout << rel_users_arr[i] << " ";
    }
    cout << endl;
    
    
    relatedUsers u;
    u.rel_users = rel_users_arr;
    u.user = user;
    u.rel_users_size = nusers;
    return u;
}

struct relatedItems{
    int *relatedItems;
    int *item;
};

relatedItems getRelatedItems(int user, int item, int** matrix, int nusers, int nitems, relatedUsers rel_use){
    int *rel_items = (int*) malloc(nitems*sizeof(int));
    int veclen = 8;
    int leftOverFromArrayEnd = nitems % veclen;
    for (int i = 0; i < nitems; i++)
    {
        rel_items[i] = -1;
    }

    __m256i zeromask = _mm256_set1_epi32(0);

    for (int related_user = 0; related_user < rel_use.rel_users_size; related_user++)
    {
        __m256i uservec = _mm256_set1_epi32(related_user);

        for (int item = 0; item < nitems-leftOverFromArrayEnd; item += veclen)
        {
            /* code */
        }
        
    }
    
    

}


/*
Code for getting related items (non vectorized, slow)

*/

// for(int i = 0; i < rel_use.rel_users_size; i++){
//         for(int j = 0; j < nitems; j++){
            
//             int user2 = rel_use.rel_users[i];
            
//             if(matrix[user2][j] != 0 && matrix[usr][j] != 0){
//                 //cout << "entering isalreadyin" << endl;
//                 bool isalready = isalreadyin(rel_items,relitemssize+1,j);
//                 //cout << "exited isalreadyin" << endl;
//                     if(!isalready) 
//                     rel_items[relitemssize] = j;
//                     ++relitemssize;
//                     //temp.push_back(j);
//             }
//         }
//     }