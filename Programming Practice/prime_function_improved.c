#include <stdio.h>
#include <math.h>

int is_prime(long n);

int main(){
    long number = 100;
    int counter = 1;
    
    for(long i = 1; i <= number; i++){
        if(is_prime(i)){
            printf("%ld %4d\n", i, counter);
            counter++;
        }
        
    }

    return 0;
}


int is_prime(long n){
    if(n < 2){
        return 0;
    }

    long limit = (long)sqrt(n);
    for(long i = 2; i <= limit; i++){
        if(n % i == 0){
            return 0;
        }
    }
    return 1;
}
