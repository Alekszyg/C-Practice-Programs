#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define RANGE 1000

struct time_variables
{
    float time_increment;
    float final_time;

    int trials;

    // time variables for time_until_rest function
    float time_velocity_zero; // time @ velocity = 0

    bool velocity_zero_check;

    // time variables for time_until_displacement_zero function
    float time_displacement_zero_1; // time 1 @ displacement = 0
    float time_displacement_zero_2; // time 2 @ displacement = 0
    
};

struct motion_variables
{
    float acceleration;
    float initial_velocity;
    float initial_displacement;

    // motion variables for time_until_rest function
    float displacement_velocity_zero; // displacement @ velocity = 0


    // motion variables for time_until_displacement_zero function
    float velocity_displacement_zero_1; // velocity 1 @ displacement = 0
    float velocity_displacement_zero_2; // velocity 2 @ displacement = 0


    float velocities[RANGE];
    float displacements[RANGE];
};

float quadratic_solver(float, float, float, bool);

void initial_input(bool, struct motion_variables *motion);
void initial_motion_values_display(struct motion_variables motion);

void simulation_input(bool, char*, struct time_variables *time);
void simulation(char, struct motion_variables *motion, struct time_variables);
void simulation_display(bool, bool, char, struct motion_variables, struct time_variables);

void time_until_rest(struct motion_variables *motion, struct time_variables *time);
void time_until_displacement_zero(struct motion_variables *motion, struct time_variables *time);
void zero_point_values_display(struct motion_variables, struct time_variables);

void restart(bool);


int main()
{
    // acceleration, initial velocity, initial displacement respectively
    struct motion_variables mv = {10, -50, 10};

    // time increment, final time
    struct time_variables time = {0.1, 20, time.final_time / time.time_increment};

    char simulate_choice = 'y';

    bool do_user_input = false;
    bool do_simulation_display_input = true;


    // gets the initial input from the user for the necessary calculations and displays them
    initial_input(do_user_input, &mv);
    initial_motion_values_display(mv);

    // gets the input from the user necessary for the simulation calculations
    simulation_input(do_user_input, &simulate_choice, &time);

    // simulates the objects movement through time
    simulation(simulate_choice, &mv, time);

    // checks if initial velocity and acceleration oppose. i.e. velocity will reach zero
    time_until_rest(&mv, &time);

    // condition if acceleration is NOT equal to zero
    time_until_displacement_zero(&mv, &time);

    // displays the values of the variables when other variables are equal to zero
    zero_point_values_display(mv, time);

    // reads the velocity and displacement values from the simulation
    simulation_display(do_user_input, do_simulation_display_input, simulate_choice, mv, time);
    
    // restarts the program
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
void initial_input(bool do_user_input, struct motion_variables *motion)
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
void simulation_input(bool do_user_input, char* simulate_choice, struct time_variables *time)
{
    if (do_user_input)
    {
        printf("\nDo you wish to simulate the object's movement through time? (y/n): ");
        scanf(" %c", simulate_choice);

        if (*simulate_choice == 'y' || *simulate_choice == 'Y')
        {
            printf("\nEnter time increment: ");
            scanf("%f", &time->time_increment);

            printf("\nEnter final time: ");
            scanf("%f", &time->final_time);

    	    time->trials = time->final_time / time->time_increment;

        } // end inner if

    } // end outer if

} // end function


// function that calculates an objects movement through time from an initial acceleration, velocity and displacement
void simulation(char simulate_choice, struct motion_variables *motion, struct time_variables time)
{
    if (simulate_choice == 'y' || simulate_choice == 'Y')
    {

        // simulates object movement, taking its acceleration, initial velocity, and initial displacement into account
        for (int i = 0; i < time.trials + 1; i++)
        {
            float time_elapsed = time.time_increment * i;
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


// function that calculates the time when velocity is zero - variables @ velocity zero point
void time_until_rest(struct motion_variables *motion, struct time_variables *time)
{
    if ((motion->initial_velocity * motion->acceleration) < 0)
    {
        time->velocity_zero_check = true;

        float time_until_velocity_zero = fabs(motion->initial_velocity) / fabs(motion->acceleration);
        float current_displacement = motion->initial_displacement + (motion->initial_velocity / 2) * time_until_velocity_zero;

        // time @ velocity zero point
        time->time_velocity_zero = time_until_velocity_zero;

        // displacement @ velocity zero point
        motion->displacement_velocity_zero = current_displacement;

    } // end if
    else
    {
        time->velocity_zero_check = false;

    } // end else

} // end function


// function that calculates the time when displacement is zero - variables @ displacement zero point
void time_until_displacement_zero(struct motion_variables *motion, struct time_variables *time)
{

    if (motion->acceleration != 0)
    {
        float time1 = quadratic_solver(motion->acceleration * -1, motion->initial_velocity * -2, motion->initial_displacement * -2, 1);
        float time2 = quadratic_solver(motion->acceleration * -1, motion->initial_velocity * -2, motion->initial_displacement * -2, 0);

        // times @ displacement zero point
        time->time_displacement_zero_1 = time1;
        time->time_displacement_zero_2 = time2;

        // velocities @ displacement zero point
        motion->velocity_displacement_zero_1 = motion->initial_velocity + (motion->acceleration * time1);
        motion->velocity_displacement_zero_2 = motion->initial_velocity + (motion->acceleration * time2);

    } // end if
    // condition if acceleration is equal to zero and velocity points towards displacement zero
    else if ((motion->initial_displacement * motion->initial_velocity) < 0)
    {
        float time_until_displacement_zero = fabs(motion->initial_displacement) / fabs(motion->initial_velocity);

        time->time_displacement_zero_1 = time_until_displacement_zero;
        motion->velocity_displacement_zero_1 = motion->initial_velocity;

    } // end else if
    
} // end function


// displays the initial motion values: acceleration, initial velocity and initial displacement
void initial_motion_values_display(struct motion_variables motion)
{
    printf("\nInitial Values:");
    printf("\nThe acceleration is: %.2f m/s^2", motion.acceleration);
    printf("\nThe initial velocity is: %.2f m/s", motion.initial_velocity);
    printf("\nThe initial displacement is: %.2f m\n", motion.initial_displacement);

} // end function


// displays the values of the variables when other variables are equal to zero
void zero_point_values_display(struct motion_variables motion, struct time_variables time)
{
    // display time_until_rest variables
    if (time.velocity_zero_check)
    {
        printf("\nWhen Velocity is Zero Values:");
        printf("\nThe time at rest is: %.2f s", time.time_velocity_zero);
        printf("\nThe displacement at rest is: %.2f m\n", motion.displacement_velocity_zero);
    }
    else
    {
        printf("\nRest is never reached\n");
    }

    // display time_until_displacement_zero variables
    if (time.time_displacement_zero_1 > 0 || time.time_displacement_zero_2 > 0)
    {
        printf("\nWhen Displacement is Zero Values:");
    }

    if (time.time_displacement_zero_1 > 0)
    {
        printf("\nThe time when displacement is zero is: %.2f s", time.time_displacement_zero_1);
        printf("\nThe velocity when displacement is zero is: %.2f m/s\n", motion.velocity_displacement_zero_1);
    }

    if (time.time_displacement_zero_2 > 0)
    {
        printf("\nThe 2nd time when displacement is zero is: %.2f s", time.time_displacement_zero_2);
        printf("\nThe 2nd velocity when displacement is zero is: %.2f m/s\n", motion.velocity_displacement_zero_2);
    }

    if (time.time_displacement_zero_1 <= 0 && time.time_displacement_zero_2 <= 0)
    {
        printf("\n\nThe time when displacement is zero is never reached");
    }

} // end function


//function for reading the values from the simulation
void simulation_display(bool do_user_input, bool do_simulation_display_input , char simulate_choice, struct motion_variables motion, struct time_variables time)
{
    if (do_user_input || do_simulation_display_input)
    {

        if (simulate_choice == 'y' || simulate_choice == 'Y')
        {
            float time_elapsed = 0;
            printf("\n\nEnter a negative number to exit.");
            printf("\nOtherwise, enter any time between 0s and %.2fs to find out the object's velocity and displacement at that very moment!: ", time.final_time);

            scanf("%f", &time_elapsed);
    	    int index = time_elapsed / time.time_increment;
            
            if (time_elapsed >= 0)
            {
                initial_motion_values_display(motion);
                zero_point_values_display(motion, time);

                printf("\n\nSimulation Values:");
                printf("\nThe velocity at %.2fs is: %.2f m/s", time_elapsed, motion.velocities[index]);
                printf("\nThe displacement at %.2fs is: %.2f m", time_elapsed, motion.displacements[index]);
                
            } // end if
            else
            {
                simulate_choice = 'n';
            } // end else

            // restarts the function
            simulation_display(do_user_input, do_simulation_display_input, simulate_choice, motion, time);
        
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
