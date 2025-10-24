#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define RANGE 1000

struct motion_values
{
    float acceleration;
    float initial_velocity;
    float initial_displacement;

    float velocities[RANGE];
    float displacements[RANGE];
};

float quadratic_solver(float, float, float, bool);

void initial_input(bool, struct motion_values *motion);
void simulation_input(bool, char*, float*, float*);

void simulation(char, struct motion_values *motion, float, float);
void simulation_read(bool, char, struct motion_values, float, float);

void time_until_rest(struct motion_values);
void time_until_displacement_zero(struct motion_values);

void restart(bool);


int main()
{
    struct motion_values mv = {-9.81, 0, 100};

    bool do_user_input = true;
    char simulate_choice = ' ';

    float time_increment = 0;
    float final_time = 0;

    // gets the initial input from the user for the calculations
    initial_input(do_user_input, &mv);

    // gets the input from the user necessary for the simulation calculation
    simulation_input(do_user_input, &simulate_choice, &time_increment, &final_time);

    // simulates the objects movement through time
    simulation(simulate_choice, &mv, time_increment, final_time);

    // checks if initial velocity and acceleration oppose. i.e. velocity will reach zero
    time_until_rest(mv);

    // condition if acceleration is NOT equal to zero
    time_until_displacement_zero(mv);

    // search through time for velocity and displacement
    simulation_read(do_user_input, simulate_choice, mv, time_increment, final_time);
    
    // program restart
    restart(do_user_input);

} // end main()


// function that solves quadratics
float quadratic_solver(float a, float b, float c, bool sign)
{
    float discriminant = pow(b, 2) - (4 * a * c);
    if (discriminant < 0)
    {
        return -1;
    }
    return (-(b) + (sign ? 1 : -1) * sqrt(discriminant)) / (2 * a);

} // end function


// function that gets the inital input from the user
void initial_input(bool do_user_input, struct motion_values *motion)
{
    if (do_user_input)
    {
        printf("\nEnter acceleration: ");
        scanf("%f", &motion -> acceleration);

        printf("\nEnter initial velocity: ");
        scanf("%f", &motion -> initial_velocity);

        printf("\nEnter initial displacement: ");
        scanf("%f", &motion -> initial_displacement);

    } // end if
    
} // end function


// function that gets the input from the user of the simulation
void simulation_input(bool do_user_input, char* simulate_choice, float* time_increment, float* final_time)
{
    if (do_user_input)
    {
        printf("\nDo you wish to simulate the object's movement through time? (y/n): ");
        scanf(" %c", simulate_choice);

        if (*simulate_choice == 'y' || *simulate_choice == 'Y')
        {
            printf("\nEnter time increment: ");
            scanf("%f", time_increment);

            printf("\nEnter final time: ");
            scanf("%f", final_time);
        } // end inner if

    } // end outer if

} // end function


// function that calculates the simulation
void simulation(char simulate_choice, struct motion_values *motion, float time_increment, float final_time)
{
    if (simulate_choice == 'y' || simulate_choice == 'Y')
    {
        const int trials = final_time / time_increment;

        // simulates object movement, taking its acceleration, initial velocity, and initial displacement into account
        for (int i = 0; i < trials + 1; i++)
        {
            float time_elapsed = time_increment * i;
            float current_velocity = motion->initial_velocity + (motion->acceleration * time_elapsed);
            float average_velocity = (current_velocity + motion->initial_velocity) / 2;
            float current_displacement = motion->initial_displacement + (average_velocity * time_elapsed);

            if (i < RANGE)
            {
                motion->velocities[i] = current_velocity;
                motion->displacements[i] = current_displacement;
            }

        } // end for

    } // end if

} // end function


// function that calculates the time when velocity is zero
void time_until_rest(struct motion_values motion)
{
    if ((motion.initial_velocity * motion.acceleration) < 0)
    {
        float time_until_velocity_zero = fabs(motion.initial_velocity) / fabs(motion.acceleration);
        float current_displacement = motion.initial_displacement + (motion.initial_velocity / 2) * time_until_velocity_zero;

        printf("\n\nTime until rest: %f seconds", time_until_velocity_zero);
        printf("\nDisplacement at rest: %f\n", current_displacement);

    } // end if
    else
    {
        printf("\n\nVelocity zero is never reached");
    } // end else

} // end function


void time_until_displacement_zero(struct motion_values motion)
{
    int found_positive_time = 0;
    if (motion.acceleration != 0)
    {
        float time1 = quadratic_solver(motion.acceleration * -1, motion.initial_velocity * -2, motion.initial_displacement * -2, 1);
        float time2 = quadratic_solver(motion.acceleration * -1, motion.initial_velocity * -2, motion.initial_displacement * -2, 0);

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
    else if ((motion.initial_displacement * motion.initial_velocity) < 0)
    {
        float time_until_displacement_zero = fabs(motion.initial_displacement) / fabs(motion.initial_velocity);
        printf("\nTime until displacement zero: %f seconds", time_until_displacement_zero);
        found_positive_time = 1;
    } // end else if

    // displays that there is no displacement zero if time is to be positive.
    if (!found_positive_time)
    {
        printf("\nDisplacement zero is never reached");
    } // end if

} // end function


//function for reading the values from the simulation
void simulation_read(bool do_user_input, char simulate_choice, struct motion_values motion, float time_increment, float final_time)
{
    if (do_user_input)
    {

        if (simulate_choice == 'y' || simulate_choice == 'Y')
        {
            float time = 0;
            printf("\n\nEnter a negative number to exit.");
            printf("\nOtherwise, enter any time between 0s and %.2fs to find out the object's velocity and displacement at that very moment!: ", final_time);

            scanf("%f", &time);

            
            if (time >= 0)
            {
                int index = time / time_increment;
                printf("\nThe velocity at %.2fs is: %.2f", time, motion.velocities[index]);
                printf("\nThe displacement at %.2fs is: %.2f", time, motion.displacements[index]);
                printf("\nThe acceleration at %.2fs is: %.2f", time, motion.acceleration);

            } // end if
            else
            {
                simulate_choice = 'n';
            } // end else

            // restarts the function
            simulation_read(do_user_input, simulate_choice, motion, time_increment, final_time);
        
        } // end inner if

    } // end outer if

} // end function


// function for restarting the program
void restart(bool do_user_input)
{
    if (do_user_input)
    {
        char restart_choice;
        printf("\nDo you wish to restart the program? (y/n): ");
        scanf(" %c", &restart_choice);

        if (restart_choice == 'y' || restart_choice == 'Y')
        {
            main();
        }
        else 0;
    } // end outer if
    
} // end function
