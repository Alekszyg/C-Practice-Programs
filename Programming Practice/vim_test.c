#include <stdio.h>



int main() 
{
    float gallons[5];
    float miles[5];
    float mpg[5];


    // reads values for gallons and miles from user
    for(int i = 0; i < 5; i++)
    {
        printf("\nEnter miles value %d: ", i + 1);
        scanf("%f", &miles[i]);
 
        printf("Enter gallons value %d: ", i + 1);
        scanf("%f", &gallons[i]);

        mpg[i] = miles[i] / gallons[i];

    }

   
    // displays the mile per gallon values
    for(int i = 0; i < 5; i++)
    {
        printf("\nthe miles per gallon for value %d is: %f",i + 1, mpg[i]);


    }

    float highest = mpg[0];
    float lowest = mpg[0];
    
    
    
    // calculates the highest and lowest value in mpg
    for(int i = 0; i < 5; i++)
    {
        if(mpg[i] > highest)
        {
            highest = mpg[i];
        }

        if(mpg[i] < lowest)
        {
            lowest = mpg[i];
        }
 
    }

    printf("\nThe Highest value in mpg is: %f", highest);
    printf("\nThe Lowest value in mpg is: %f", lowest);


    return 0;
} 

 














 
















