#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int random = rand() % 101;
    int user_input = -1;
    printf("\nGuess the number! It's between 0 and 100.\n\n");
    
    while(user_input != random)
    {
        printf("\nEnter your guess: ");
        int result = scanf("%d", &user_input);

        if (result != 1)
        {
            printf("Invalid input! Please enter a valid number.");
            while (getchar() != '\n');  // Clear invalid input from buffer
            continue;
        }    
        
        if(user_input > random)
        {
            printf("Your guess is too high!\n\n");
        }
        
        else if (user_input < random)
        {
            printf("Your guess is too low!\n\n");
        }
    }
    printf("That's right! The number is %d\n", random);
    return 0;
}