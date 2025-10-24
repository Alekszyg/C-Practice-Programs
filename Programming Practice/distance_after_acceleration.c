#include <stdio.h>
#include <math.h>

float positive_quadratic_solver(float, float, float);
float negative_quadratic_solver(float, float, float);
float time_till_displacement_zero(float, float, float);

int main()
{
    float time_increment = 1;
    float final_time = 30;

    int trials = final_time / time_increment;
    
    float acceleration = -9.81;

    float initial_velocity = 0;

    float initial_displacement = 100;

    float current_velocity = 0;
    float current_displacement = 0;

    for (int i = 0; i < trials + 1; i++)
    {
        float time_elapsed = time_increment * i;

        float current_velocity = initial_velocity + (acceleration * time_elapsed);

        float average_velocity = (current_velocity + initial_velocity) / 2;

        float current_displacement = initial_displacement + (average_velocity * time_elapsed);

        printf("\nTime Elapsed: %f seconds", time_elapsed);
        printf("\nCurrent Velocity: %f", current_velocity);
        printf("\nCurrent Displacement: %f\n", current_displacement);
    } // end for

    // condition if initial velocity and acceleration oppose. i.e. velocity will eventually reach zero
    if ((initial_velocity * acceleration) < 0)
    {
        float time_till_rest = fabs(initial_velocity) / fabs(acceleration);

        current_displacement = initial_displacement + (initial_velocity / 2) * time_till_rest;

        printf("\n\nTime Till Rest: %f seconds", time_till_rest);
        printf("\nDisplacement: %f\n", current_displacement);

    } // end for

    // condition if initial displacement is zero
    if (initial_displacement == 0)
    {
        // condition if initial velocity and acceleration oppose each other
        if ((initial_velocity * acceleration) < 0)
        {
            float time_until = time_till_displacement_zero(acceleration, initial_velocity, initial_displacement);

            printf("\nTime Till Displacement Zero: %f seconds", time_until);
        }
    }
    // condition if initial displacement is not zero and velocity points away from displacement zero
    else if ((initial_displacement * initial_velocity) >= 0)
    {
        // condition if acceleration points towards displacement zero
        if ((initial_displacement * acceleration) < 0)
        {
            float time_until = time_till_displacement_zero(acceleration, initial_velocity, initial_displacement);

            printf("\nTime Till Displacement Zero: %f seconds", time_until);
        }
    }
    // condition if initial displacement is not zero and velocity points towards displacement zero
    else
    {
        // condition if acceleration points towards displacement zero
        if ((initial_displacement * acceleration) < 0)
        {

            float time1 = positive_quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2);
            float time2 = negative_quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2);
            float time_till_displacement_zero = (time1 > 0) ? time1 : (time2 > 0) ? time2
                                                                                  : 0;
            printf("\nTime Till Displacement Zero: %f seconds", time_till_displacement_zero);
        }
        // condition if acceleration points away from displacement zero
        else if ((initial_displacement * acceleration) > 0)
        {
            float time1 = positive_quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2);
            float time2 = negative_quadratic_solver(acceleration * -1, initial_velocity * -2, initial_displacement * -2);

            if (time1 > 0)
            {
                printf("\nTime Till Displacement Zero: %f seconds", time1);
            }                                                       
            if (time2 > 0)
            {
                printf("\nTime Till Displacement Zero: %f seconds", time2);
            }   
        }
        // condition if acceleration is zero
        else if (acceleration == 0)
        {
            float time_till_displacement_zero = fabs(initial_displacement) / fabs(initial_velocity);
            printf("\nTime Till Displacement Zero: %f seconds", time_till_displacement_zero);
        }
    }

    return 0;
} // end main()




float positive_quadratic_solver(float a, float b, float c)
{
    float x = (-(b) + sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
    return x;
}

float negative_quadratic_solver(float a, float b, float c)
{
    float x = (-(b)-sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
    return x;
}

float time_till_displacement_zero(float acceleration, float initial_velocity, float initial_displacement)
{
    float time_till_rest = fabs(initial_velocity) / fabs(acceleration);
    float current_displacement = initial_displacement + (initial_velocity / 2) * time_till_rest;
    float time = (sqrt((fabs(current_displacement * 2)) / fabs(acceleration)) + time_till_rest);
    return time;
}