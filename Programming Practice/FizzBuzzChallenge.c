#include <stdio.h>

int main(){

    for(int i = 1; i < 101; i++){
        if(i % 3 == 0 && i % 5 == 0){
            printf("\nFizzBuzz");
        }
        else if (i % 5 == 0)
        {
            printf("\nBuzz");
        }
        else if (i % 3 == 0)
        {
            printf("\nFizz");
        }
        else{
            printf("\n%d", i);
        }
        
    } // end for


    return 0;
} // end main()