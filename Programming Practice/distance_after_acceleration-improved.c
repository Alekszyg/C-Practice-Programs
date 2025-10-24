#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define RANGE 1000

float quadratic_solver(float, float, float, bool);

int main()
{
    char restart = ' ';
    char simulate = ' ';
    int found_positive_time = 0;

    const float time_increment = 0;
    const float final_time = 0;

    const float acceleration = 0;

    const float initial_velocity = 0;
    float velocities[RANGE];

    const float initial_displacement = 0;
    float displacements[RANGE];

    printf("\nEnter acceleration: ");
    scanf("%f", &acceleration);

    printf("\nEnter initial velocity: ");
    scanf("%f", &initial_velocity);

    printf("\nEnter initial displacement: ");
    scanf("%f", &initial_displacement);

    printf("\nDo you wish to simulate the object's movement through time? (y/n): ");
    scanf(" %c", &simulate);

    if (simulate == 'y' || simulate == 'Y')
    {
        printf("\nEnter time increment: ");
        scanf("%f", &time_increment);

        printf("\nEnter final time: ");
        scanf("%f", &final_time);

        const int trials = final_time / time_increment;

        // simulates object movement, taking its acceleration, initial velocity, and initial displacement into account
        for (int i = 0; i < trials + 1; i++)
        {
            float time_elapsed = time_increment * i;
            float current_velocity = initial_velocity + (acceleration * time_elapsed);
            float average_velocity = (current_velocity + initial_velocity) / 2;
            float current_displacement = initial_displacement + (average_velocity * time_elapsed);

            if (RANGE > i)
            {
                velocities[i] = current_velocity;
                displacements[i] = current_displacement;
            }
            /*
            printf("\nTime Elapsed: %f seconds", time_elapsed);
            printf("\nCurrent Velocity: %f", current_velocity);
            printf("\nCurrent Displacement: %f\n", current_displacement);
            */

        } // end for
    }

    // check if initial velocity and acceleration oppose. i.e. velocity will reach zero
    if ((initial_velocity * acceleration) < 0)
    {
        float time_until_rest = fabs(initial_velocity) / fabs(acceleration);
        float current_displacement = initial_displacement + (initial_velocity / 2) * time_until_rest;

        printf("\n\nTime until rest: %f seconds", time_until_rest);
        printf("\nDisplacement at rest: %f\n", current_displacement);

    } // end if

    // condition if acceleration is NOT equal to zero
    if (acceleration != 0)
    {
        float time1 = quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2, 1);
        float time2 = quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2, 0);

        // checks if time is positive
        if (time1 > 0)
        {
            printf("\nTime until displacement zero: %f seconds", time1);
            found_positive_time = 1;
        }

        if (time2 > 0)
        {
            printf("\nTime until displacement zero: %f seconds", time2);
            found_positive_time = 1;
        }

    } // end if

    // condition if acceleration is equal to zero and velocity points towards displacement zero
    else if ((initial_displacement * initial_velocity) < 0)
    {
        float time_until_displacement_zero = fabs(initial_displacement) / fabs(initial_velocity);
        printf("\nTime until displacement zero: %f seconds", time_until_displacement_zero);
        found_positive_time = 1;
    } // end else if

    // displays that there is no displacement zero if time is to be positive.
    if (!found_positive_time)
    {
        printf("\nDisplacement zero is never reached");
    }

    // search through time for velocity and displacement
    while (simulate == 'y' || simulate == 'Y')
    {
        char exit_choice;
        float time = 0;
        printf("\notherwise, enter any time between 0s and %.2fs to find out the object's velocity and displacement at that very moment!: ", final_time);

        scanf("%f", &time);

        printf("\nThe velocity at %.2fs is: %.2f", time, velocities[(int)(time / time_increment)]);
        printf("\nThe displacement at %.2fs is: %.2f", time, displacements[(int)(time / time_increment)]);
        printf("\nThe acceleration at %.2fs is: %.2f", time, acceleration);

        printf("\nDo you wish to exit? (y/n): ");
        scanf(" %c", &exit_choice);

        if (exit_choice == 'y' || exit_choice == 'Y')
        {
            simulate = 'n';
        }
    }
    // program restart
    printf("\nDo you wish to restart the program? (y/n): ");
    scanf(" %c", &restart);

    if (restart == 'y' || restart == 'Y')
    {
        main();
    }
    else 0;

} // end main()

float quadratic_solver(float a, float b, float c, bool sign)
{
    float discriminant = pow(b, 2) - (4 * a * c);
    if (discriminant < 0)
    {
        return -1;
    }
    return (-(b) + (sign ? 1 : -1) * sqrt(discriminant)) / (2 * a);
} // end function
