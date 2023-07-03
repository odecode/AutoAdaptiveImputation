void sort_test_users(int** testusers){
    for (int i = 0; i < 20000; i++)
    {
        for (int j = 0; j < 20000; j++)
        {
            if(testusers[i][0] > testusers[j][0]){
                int* temp = testusers[i];
                testusers[i] = testusers[j];
                testusers[j] = temp;
            }
        }
        
    }
    
}