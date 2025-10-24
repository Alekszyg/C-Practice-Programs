#include <stdio.h>
#include <math.h>
#include <stdbool.h>

/*
program to return true value when prime, and false value when non prime.
*/

bool is_prime(long number);

int main()
{
    long number = 1000000001;
    if(is_prime(number))
    {
        printf("\n%ld is a prime\n", number);
    }
    else
    {
        printf("\n%ld is not a prime\n", number);
    }
}


bool is_prime(long number)
{
    if(number == 1)
        return false;
    if(number == 2)
        return true;
    if(number % 2 == 0)
        return false;
    
    long counter = 2;

    while(counter < number)
    {
        if(number % counter == 0)
        {
            return false;
        }
        counter++;
    }
    return true;
}