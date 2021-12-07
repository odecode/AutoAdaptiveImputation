#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
using namespace std;

// const int nusers = 943; // number of users in matrix
// const int nitems = 1682;  // number of items in matrix
// const int lines = 100000;   //number of ratings in data set//


struct relatedUsers{
    int *rel_users;
    int rel_users_size;
    int *user;
};




relatedUsers get_related_users(int *user, int *item, int** matrix, int nusers, int nitems){
    //cout << "in get rel users 1\n";
    
    /*
    Find all related users to target user
    Related users are all users who have rated target item
    TODO: last argument not used; need to refactor
    */
    //int* rel_users_arr[nusers] = {0};
    int *rel_users_arr = (int*) malloc(nusers*sizeof(int));
    for (int i = 0; i < nusers; i++)
    {
        rel_users_arr[i] = -1;
    }
    //cout << "in get rel users 2\n";
    int reluserssize = 0;
    // i == "user2"
    for(int i = 0; i < nusers; i++){
        bool related = true;
        if(*user != i){
        //cout << "in get rel users 3\n";
            related = matrix[i][*item] == 0 ? false : true;
           // related = matrix[i*nitems+item] == 0 ? false : true;
            //if(matrix[i][item] == 0){related = false;}
            //cout << related << endl;
            if(related){
                //cout << "in get rel users 4\n";
                rel_users_arr[reluserssize]=i;
                reluserssize++;
                //cout << "in get rel users 5\n";
            }
        }
    }

    relatedUsers rel_users;
    rel_users.rel_users = (int*) malloc(reluserssize*sizeof(int));
    rel_users.user = user;
    for(int i = 0; i < reluserssize; i++){
        rel_users.rel_users[i] = rel_users_arr[i];
        //cout << "related users " << i << " = " << rel_users.rel_users[i] << endl;
    }
    //free(rel_users_arr);
    rel_users.rel_users_size = reluserssize;

    return rel_users;
}

bool compareItems(int i, int j){
    return (i < j);
}

struct relatedItems{
    int* rel_items;
    int rel_items_size;
    int* item;
};

bool isalreadyin(int* arr, int arrsize, int val){
    bool isalready;
    for (int i = 0; i < arrsize; i++)
    {
        isalready = arr[i] == val ? true : false;
        //cout << arr << " " << arrsize << " " << arr[arrsize] << " " << arr[i] << endl;
        // int arrval = arr[i];
        // if(arrval == val){
        //     isalready=true;
        //     break;
        // }
        // else{isalready=false;}
    
        if(isalready) break;
    }
    return isalready;
}

relatedItems get_related_items(int* user, int* item, int** matrix, relatedUsers rel_use, int nusers, int nitems){
    //std::cout << "in get related items\n";
    /*
    Find all related items to target item
    Related items are items which target user and a related user has rated
    */
    //int rel_items[nitems] = {0};
    int* rel_items = (int*) malloc(nusers*nitems*sizeof(int));
    //vector<int> temp;
    //array<int,nitems> rel_items = = new array<int,nitems>;
    for (int i = 0; i < nusers*nitems; i++)
    {
        rel_items[i] = -1;
    }
    
    int usr = *user;
    int relitemssize = 0;
    for(int i = 0; i < rel_use.rel_users_size; i++){
        for(int j = 0; j < nitems; j++){
            int user2 = rel_use.rel_users[i];
            if(matrix[user2][j] != 0 && matrix[usr][j] != 0){
                //cout << "entering isalreadyin" << endl;
                bool isalready = isalreadyin(rel_items,relitemssize+1,j);
                //cout << "exited isalreadyin" << endl;
                    if(!isalready) 
                    rel_items[relitemssize] = j;
                    ++relitemssize;
                    //temp.push_back(j);
            }
        }
    }
    //cout << "before vector sort\n";
    //sort(temp.begin(),temp.end(),compareItems);
    int lastitem = -1;
    int relitemsnewsize = 0;
    int* rel_items_no_duplicates = (int*) malloc(relitemssize*sizeof(int));

    for (int i = 0; i < relitemssize; i++)
    {
        rel_items_no_duplicates[i] = -1;
    }
    
   for (int i = 0; i < relitemssize; i++)
   {
       //cout << "transferring from vector to rel items no duplicates\n";
       int tempitem = rel_items[i];
       if(tempitem != lastitem){
           rel_items_no_duplicates[relitemsnewsize] = tempitem;
            relitemsnewsize++;
       }
       lastitem = tempitem;
   }
   

    // turn set into vector for easier processing
    relatedItems relItems;
    relItems.rel_items = (int*) malloc(relitemsnewsize*sizeof(int));
    for(int i = 0; i < relitemsnewsize; i++){
        relItems.rel_items[i] = rel_items_no_duplicates[i];
        //cout << "related items " << i << " = " << relItems.rel_items[i] << endl;
    }
    relItems.rel_items_size = relitemsnewsize;
    relItems.item = item;
    free(rel_items);
    free(rel_items_no_duplicates);
    //free(rel_items_no_duplicates);
    //cout << "exiting rel items\n";
    return relItems;
}

struct keyNeighbors{
    struct relatedUsers rel_users;
    struct relatedItems rel_items;
    int* user;
    int* item;
};

struct allKeyNeighbors{
    struct keyNeighbors *allKeyNeighbors;
    int allkeyneighbors_size;
};

keyNeighbors get_key_neighbors(int* user, int* item, int** matrix, int size, int nusers, int nitems){
    //cout << "in get key neighbors\n";
    /*
    Find key neighbors
    Key neighbors are vectors of (user,item,rating) from related users to related items
    size parameter determines maximum size of key neighbors vector (to speed up computation)
    */
    keyNeighbors keyneighbors;
    //vector<vector<int>> key_neighbors;
    relatedUsers relUsers = get_related_users(user,item,matrix, nusers, nitems);
    relatedItems relItems = get_related_items(user,item,matrix,relUsers, nusers, nitems);
    int rel_users_size = relUsers.rel_users_size;
    int rel_items_size = relItems.rel_items_size;

    keyneighbors.rel_users=relUsers;
    keyneighbors.rel_items=relItems;
    keyneighbors.user=user;
    keyneighbors.item=item;


    return keyneighbors;
}


void generateAllKeyNeighbors(int** matrix,int size, int nusers, int nitems, int* allusers, int* allitems){
    ofstream MyFile("keyneighbors.txt");
    // allKeyNeighbors allkeyneighs;
    // allkeyneighs.allKeyNeighbors = new keyNeighbors[nusers*nitems*sizeof(keyNeighbors)];
    // allkeyneighs.allkeyneighbors_size=nusers*nitems;

    // u = user, it = item
    for (int u = 0; u < nusers; u++)
    {
        if(u%10==0){
            cout << "on user " << u << endl;
        }
        for (int it = 0; it < nitems; it++)
        {
            // int user = allusers[user];
            // int item = allitems[item];
            
            int* puser = &allusers[u];
            int* pitem = &allitems[it];
            keyNeighbors kn = get_key_neighbors(puser,pitem,matrix,size,nusers,nitems);
            kn.user = puser;
            kn.item = pitem;
            //allkeyneighs.allKeyNeighbors[u+it] = kn;
            string s = "";
            s = to_string(u) + " " + to_string(it);
            for (int relus = 0; relus < kn.rel_users.rel_users_size; relus++)
            {
                for (int relit = 0; relit < kn.rel_items.rel_items_size; relit++)
                {
                    int user = kn.rel_users.rel_users[relus];
                    int item = kn.rel_items.rel_items[relit];
                    int rating = matrix[user][item];
                    s = s + " " + to_string(rating);
                }
                
            }
            MyFile << s << endl;

            //MyFile << u << " " << it 
        }   
    }
    MyFile.close();
    //return allkeyneighs;
}