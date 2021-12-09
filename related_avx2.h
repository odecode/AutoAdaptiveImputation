#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <omp.h>
#include <immintrin.h>
using namespace std;

struct relatedUsers{
    int *rel_users;
    int rel_users_size;
    int *user;
};

relatedUsers get_related_users(int *user, int *item, int** matrix, int nusers, int nitems){
    
    /*
    Find all related users to target user
    Related users are all users who have rated target item
    */
    int *rel_users_arr = (int*) malloc(nusers*sizeof(int));
    for (int i = 0; i < nusers; i++)
    {
        rel_users_arr[i] = -1;
    }
    
    int reluserssize = 0;
    
    /* We use 32 bit int SIMD with 256-bit vector registers
        256/32 = 8 --> we can fit 8 ints at a time
        To loop through the array rel_users_arr we take modulo 8 to find out division remainder
        and loop through the even number with SIMD, and do the remainder without simd    
    */
   __m256i mask;
   for (int i = 0; i < 8; i++)
   {
       mask[i] = 0;
   }
   
    int leftOverFromArrayEnd = nusers % 8;
    for (int user2 = 0; user2 < nusers - leftOverFromArrayEnd; user2 += 8)
    {
        __m256i data;
        
        // load a vector of ratings from matrix (from user2 to user2 + 8)
        for (int i = 0; i < 8; i++)
        {
            data[i] = matrix[user2+i][*item];
        }
        
        
        //__m256i RatingVec = _mm256_load_si256(&data);
        __m256i result = _mm256_cmpeq_epi32(data,mask);
        for (int i = 0; i < 8; i++)
        {
            cout << result[i] << " ";
        }
        cout << endl;
        

    }
}

int main(){
    


    return 0;
}