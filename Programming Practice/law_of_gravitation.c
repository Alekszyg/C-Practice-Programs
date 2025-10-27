#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <windows.h>

#define MINUTE (60)
#define HOUR (MINUTE * 60)
#define DAY (HOUR * 24)
#define WEEK (DAY * 7)

// update simulation every minute
//#define delta_time (MINUTE)

int delta_time = MINUTE;

// record objects data every minute
int log_step = MINUTE;

// how often objects are rendered
int render_step = DAY;

// if true, each render each stays on screen until user dismisses
bool render_walkthrough = true;

int zoom = 1;

// the timescale of the simulation
int time_scale = (WEEK * 4);

#define MINUTE_INTERVAL (MINUTE / delta_time)
#define HOUR_INTERVAL ( HOUR / delta_time )
#define DAY_INTERVAL ( DAY / delta_time )
#define WEEK_INTERVAL ( WEEK / delta_time ) 



// distance from middle of render to the edge of render in metres
#define RENDER_SIZE 400000000 // 400 million

// pixel ratio for square grid
#define NO_PIXELSX 17 // 17
#define NO_PIXELSY 21 // 21

#define NO_OBJECTS 3

const double GRAVITATIONAL_CONSTANT = 6.67430e-11;

// enum for plane axes
enum Planes {XY, YZ, XZ};

int plane = XY;

typedef struct
{
    double x;
    double y;
    double z;

} Vec3;

typedef struct
{
    Vec3 position;
    Vec3 velocity;
    Vec3 force;
} Motion;

typedef struct
{
    double mass;
    Motion motion;
    char symbol;

} Object;

// calculates distance between two objects
double distance(Object, Object);

void apply_gravitational_forces(Object*, Object*);
void apply_gravitational_forces_N(Object[]);

void display_position(Object);

// updates an objects velocity and position based on the force acting on it
void update(Object *object);

// update all the objects velocity and position based on the forces acting on them
void update_N(Object[]);

// updates the log, a record of all the objects variables over time
void update_log(Object*, Object[], int time);

// retrieves a pointer to the object array at a given moment
Object* get_log_data(Object *sim_log, int time_seconds);

void simulate(Object *sim_log, Object objects[], int time_seconds);

void render_objects(Object[], int plane, float zoom);

void render_objects_over_time(Object *sim_log, int plane, float zoom, int start, int end);

// checks if step is at a given interval
bool is_interval(int, int);


char* display_time(int);

int program_ui(Object *sim_log, Object[]);

int settings_ui();

int simulation_ui(Object *sim_log, Object[]);

void intro();

void menu_banner(int menu);

void clear_input_buffer();

int main()
{
    Object objects[NO_OBJECTS];
    
    int rows = time_scale / log_step;
    int cols = NO_OBJECTS;

    Object *simulation_log = malloc(rows * cols * sizeof(Object));
    if (!simulation_log)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Earth
    objects[0].mass = 5.972e24;  // kg
    objects[0].motion.position = (Vec3){0.0f, 0.0f, 0.0f};
    objects[0].motion.velocity = (Vec3){0.0f, 0.0f, 0.0f};
    objects[0].motion.force = (Vec3){0.0f, 0.0f, 0.0f};
    objects[0].symbol = 'E';

    // Moon
    objects[1].mass = 7.348e22;  // kg
    objects[1].motion.position = (Vec3){384400000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[1].motion.velocity = (Vec3){0.0f, 1022.0f, 0.0f};        // m/s (orbital speed)
    objects[1].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    // moon orbital speed 1022.0f
    objects[1].symbol = 'M';

    // Satellite
    objects[2].mass = 6000;  // kg
    objects[2].motion.position = (Vec3){36000000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[2].motion.velocity = (Vec3){0.0f, 2000.0f, 2000.0f};        // m/s (orbital speed)
    objects[2].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    objects[2].symbol = 'S';

    // set initial values
    update_log(simulation_log, objects, 0);
    program_ui(simulation_log, objects);
    /*
    // i timestep = delta_time
    for (int i = 0; i < (time_scale / delta_time) + 1; i++)
    {

        // log every log_step
        update_log(simulation_log, objects, i * delta_time);
        
        // render every day
        if (is_interval(DAY_INTERVAL, i))
        {
            display_time(i * delta_time);
            render_objects(objects, XY, 1);
            display_position(objects[0]);

        }


        apply_gravitational_forces_N(objects);
        update_N(objects);
    }
    
    */
    


    //render_objects(get_log_data(simulation_log, objects, WEEK - (DAY / 2)), XY, 1);
    free(simulation_log);

    return 0;
}

double distance(Object object1, Object object2)
{
    double distanceX = object1.motion.position.x - object2.motion.position.x;
    double distanceY = object1.motion.position.y - object2.motion.position.y;
    double distanceZ = object1.motion.position.z - object2.motion.position.z;

    return sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
};


// applies the gravitational forces between two objects
void apply_gravitational_forces(Object *object1, Object *object2) {
    Vec3 r;
    r.x = object2->motion.position.x - object1->motion.position.x;
    r.y = object2->motion.position.y - object1->motion.position.y;
    r.z = object2->motion.position.z - object1->motion.position.z;

    double distance = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    double force_magnitude = (GRAVITATIONAL_CONSTANT * object1->mass * object2->mass) / (distance * distance);

    Vec3 force;
    force.x = force_magnitude * r.x / distance;
    force.y = force_magnitude * r.y / distance;
    force.z = force_magnitude * r.z / distance;


    object1->motion.force.x += force.x; 
    object1->motion.force.y += force.y; 
    object1->motion.force.z += force.z; 

    object2->motion.force.x -= force.x; 
    object2->motion.force.y -= force.y; 
    object2->motion.force.z -= force.z; 
}


// applies the gravitational forces between all objects
void apply_gravitational_forces_N(Object objects[])
{
    
    for (int i = 0; i < NO_OBJECTS; i++)
    {
        objects[i].motion.force = (Vec3){0.0f,0.0f,0.0f};
    }

    for (int i = 0; i < (NO_OBJECTS - 1); i++)
    {
        for (int j = i+1;  j < NO_OBJECTS; j++)
        {
            apply_gravitational_forces(&objects[i], &objects[j]);
        } 
    }
}


// updates the velocity and position of a given object
void update(Object *object) {
    Vec3 acceleration = {
        object->motion.force.x / object->mass,
        object->motion.force.y / object->mass,
        object->motion.force.z / object->mass
    };

    object->motion.velocity.x += acceleration.x * delta_time;
    object->motion.velocity.y += acceleration.y * delta_time;
    object->motion.velocity.z += acceleration.z * delta_time;

    object->motion.position.x += object->motion.velocity.x * delta_time;
    object->motion.position.y += object->motion.velocity.y * delta_time;
    object->motion.position.z += object->motion.velocity.z * delta_time;
}


// updates the velocity and position of all objects
void update_N(Object objects[])
{
    for (int i = 0; i < NO_OBJECTS; i++)
    {
        update(&objects[i]);
    }
}


// displays the position of a given object
void display_position(Object object)
{
    printf("\nx: %e", object.motion.position.x);
    printf("\ny: %e", object.motion.position.y);
    printf("\nz: %e\n", object.motion.position.z);

};

void display_all_information(Object objects[])
{
    for (int i = 0; i < NO_OBJECTS; i++)
    {
        printf("\n");
        for (int i = 0; i < 50; i++)
        {
            printf("-");
        }

        printf("\nObject: %c\n", objects[i].symbol);
        for (int i = 0; i < 50; i++)
        {
            printf("-");
        }

        printf("\nMass:");
        printf("\nkg: %e", objects[i].mass);

        printf("\n\nPosition:");
        printf("\nx: %e", objects[i].motion.position.x);
        printf("\ny: %e", objects[i].motion.position.y);
        printf("\nz: %e", objects[i].motion.position.z);

        printf("\n\nVelocity:");
        printf("\nx: %e", objects[i].motion.velocity.x);
        printf("\ny: %e", objects[i].motion.velocity.y);
        printf("\nz: %e", objects[i].motion.velocity.z);

        printf("\n\nForce:");
        printf("\nx: %e", objects[i].motion.force.x);
        printf("\ny: %e", objects[i].motion.force.y);
        printf("\nz: %e\n\n", objects[i].motion.force.z);

    }
};


// renders all the objects in ASCII in a given area
void render_objects(Object objects[], int plane, float zoom)
{
    char *plane_str;

    Vec3 coordinates[NO_OBJECTS];
    bool displayed = false;

    // number of pixels from the middle to the end
    int half_screen_sizeX = NO_PIXELSX / 2;
    int half_screen_sizeY = NO_PIXELSY / 2;

    int pixel_sizeX = (RENDER_SIZE / half_screen_sizeX) / zoom;
    int pixel_sizeY = (RENDER_SIZE / half_screen_sizeY) / zoom;

    for (int i = 0; i < NO_OBJECTS; i++)
    {
        if (plane == XY)
        {
            plane_str = "| X: <> | Y: v^ |";
            coordinates[i].x = (objects[i].motion.position.x / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.y / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == YZ)
        {
            plane_str = "| Y: <> | Z: v^ |";
            coordinates[i].x = (objects[i].motion.position.y / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.z / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == XZ)
        {
            plane_str = "| X: <> | Z: v^ |";
            coordinates[i].x = (objects[i].motion.position.x / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.z / pixel_sizeY) + (half_screen_sizeY);
        }
    }


    for (int y = 0; y < NO_PIXELSY; y++)
    {
        for (int x = 0; x < NO_PIXELSX; x++)
        {
            for (int ob = 0; ob < NO_OBJECTS; ob++)
            {
                if ((int)coordinates[ob].x == x && ((NO_PIXELSY - 1) - (int)coordinates[ob].y) == y)
                {
                    printf(" %c ", objects[ob].symbol);
                    displayed = true;
                    break;

                }
            }
            
            if (!displayed)
            {
                printf(" . ");
            }

            displayed = false;
        }
        
        printf("\n");
    }
    printf("%s", plane_str);

}


void render_objects_over_time(Object *sim_log, int plane, float zoom, int start, int end)
{
    clear_input_buffer();

    for (int i = (start / render_step); i < (end / render_step) + 1; i++)
    {
        printf("\n\n%s\n", display_time(i * render_step));
        render_objects(get_log_data(sim_log, i * render_step), plane, zoom);

        if (render_walkthrough)
        {
            getchar();
        }
    }
}


// writes all the objects motion data to the simulation log every log step interval
void update_log(Object *sim_log, Object objects[], int time_seconds)
{
    if (is_interval(log_step, time_seconds))
    {
        int index = (time_seconds / log_step);
        for (int i = 0; i < NO_OBJECTS; i++)
        {
            sim_log[index * NO_OBJECTS + i].motion = objects[i].motion;
            sim_log[index * NO_OBJECTS + i].mass = objects[i].mass;
            sim_log[index * NO_OBJECTS + i].symbol = objects[i].symbol;
        }
    }
    
}



Object* get_log_data(Object *sim_log, int time_seconds)
{
    int index = (time_seconds / log_step);

    return &sim_log[index * NO_OBJECTS];
}


// returns true if step is at a given time interval
bool is_interval(int interval, int step)
{
    if (interval == 0)
    {
        return false;
    }

    return step % interval == 0;
}


// converts the current time in seconds to a human readable time format
char* display_time(int time_seconds)
{
    static char time_str[60];
    int days = 0;
    int hours = 0;
    int minutes = 0;

    if (DAY_INTERVAL > 0)
    {
        days = time_seconds / DAY;
    }

    if (HOUR_INTERVAL > 0)
    {
        if (DAY_INTERVAL > 0)
            hours = (time_seconds % DAY) / HOUR;
        else
            hours = time_seconds / HOUR;
    }

    if (HOUR_INTERVAL > 0)
    {
        minutes = time_seconds % HOUR;
    }
    else
    {
        minutes = time_seconds;  // if HOUR_INTERVAL is 0, just show total steps as minutes
    }

    //printf("\n| DAY: %d | HOUR: %d | MINUTE: %d |\n", days, hours, minutes);
    snprintf(time_str, sizeof(time_str), "| DAY: %d | HOUR: %d | MINUTE: %d |", days, hours, minutes);
    return time_str;
}


int program_ui(Object *sim_log, Object objects[])
{
    intro();
    int user_choice = 0;
    do
    {
        printf("\nHere are your options:\n");
        printf("  - Start simulation (1)\n");
        printf("  - Adjust settings (2)\n");
        printf("  - Exit the program (-1)\n");
        scanf("%d", &user_choice);

        if (user_choice == 1)
        {
            simulation_ui(sim_log, objects);
        }
        else if (user_choice == 2)
        {
            settings_ui();
        }

    } while (user_choice != -1);
    
    return 0;
}


void intro()
{   
    int border_length = 50;
    for (int i = 0; i < border_length; i++)
    {
        printf("-");
    }

    printf("\n\n\n\t      | Gravity Simulation |\n\n\n");

    for (int i = 0; i < border_length; i++)
    {
        printf("-");
    }

    printf("\nBy Aleks Zygarlowski\n\n\n");
}


int simulation_ui(Object *sim_log, Object objects[])
{
    int user_choice;

    menu_banner(1);

    do
    {
        printf("\nHere are your options:\n");
        printf("  - Display initial simulation state (1)\n");
        printf("  - Run simulation for a period (2)\n");
        printf("  - Render simulation for a period (3)\n");
        printf("  - Return to main menu (-1)\n");

       

        scanf("%d", &user_choice);

        if (user_choice == 1)
        {
            printf("\n%s\n", display_time(0));
            render_objects(&sim_log[0 * NO_OBJECTS], XY, 1);
            display_all_information(&sim_log[0 * NO_OBJECTS]);
        }
        else if (user_choice == 2)
        {
            user_choice = 0;
            int time_seconds, days, hours, minutes;
            printf("\nThe current delta time is: %d seconds", delta_time);
            printf("\nThe current log step is: %d seconds", log_step);
            printf("\nHow long do you want to run the simulation for? Enter in the format: days hours minutes (e.g., 7 0 0):\n");
            scanf("%d %d %d", &days, &hours, &minutes);
            time_seconds = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);
            time_scale = time_seconds;
            simulate(sim_log, objects, time_seconds);
            printf("\nSimulation successfully ran for %s\n", display_time(time_seconds));
        }
        else if (user_choice == 3)
        {
            int time_seconds_start, time_seconds_end, days, hours, minutes;
            printf("\nThe current delta time is: %d seconds", delta_time);
            printf("\nThe current log step is: %d seconds", log_step);
            printf("\nThe current render step is: %s", display_time(render_step));
            printf("\nThe simulation has ran for: %s", display_time(time_scale)); 
            printf("\nBetween what two times do you want to render the simulation for? Enter in the format: days hours minutes (e.g., 7 0 0):\n");

            printf("Start: ");
            scanf("%d %d %d", &days, &hours, &minutes);
            time_seconds_start = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);

            printf("\nEnd: ");
            scanf("%d %d %d", &days, &hours, &minutes);
            time_seconds_end = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);
            
            render_objects_over_time(sim_log, plane, zoom, time_seconds_start, time_seconds_end);
        }

    } while (user_choice != -1);
    
    menu_banner(0);
}


int settings_ui()
{
    int time_seconds, days, hours, minutes;
    int user_choice;
    menu_banner(2);

    do
    {
        printf("\nHere are your options:\n");
        printf("  - Adjust simulation settings (1)\n");
        printf("  - Adjust rendering settings (2)\n");
        printf("  - Return to main menu: (-1)\n");

        scanf("%d", &user_choice);

        if (user_choice == 1)
        {
            
            printf("\nHere are your options:\n");
            printf("  - Adjust delta time (1)\n");
            printf("  - Adjust log step (2)\n");
            printf("  - Return to previous menu (-1)\n");

            scanf("%d", &user_choice);
            
            if (user_choice == 1)
            {
                
                printf("\nDelta time refers to how often the simulation is updated. It can be thought of as the accuracy of the simulation\n");
                printf("The current delta time is %d seconds, meaning the simulation updates every %d seconds", delta_time, delta_time);
                printf("\nWhat do you want delta time to be? Enter in the format: days hours minutes (e.g., 7 0 0):\n");
                scanf("%d %d %d", &days, &hours, &minutes);

                time_seconds = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);

                if (time_seconds == 0)
                {
                    delta_time = MINUTE;
                }
                else
                {
                    if (time_seconds > log_step)
                    {
                        log_step = time_seconds;
                    }
                    delta_time = time_seconds;
                }
                
                printf("\ndelta time reassigned successfully! Delta time is: %d seconds\n", delta_time);

            }
            else if (user_choice == 2)
            {
                printf("\nLog step refers to how often an entry is written into the simulation record\n");
                printf("The current log step is %d seconds, meaning the simulation record is written into every %d seconds", log_step, log_step);
                printf("\nWhat do you want log step to be? Enter in the format: days hours minutes (e.g., 7 0 0):\n");
                scanf("%d %d %d", &days, &hours, &minutes);

                time_seconds = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);
                if (time_seconds < delta_time || time_seconds == 0)
                {
                    log_step = delta_time;
                }
                else
                {
                    log_step = time_seconds;
                }
                
                printf("\nlog step reassigned successfully! log step is: %d seconds\n", log_step);
            }

            user_choice = 0;
            

        }
        else if (user_choice == 2)
        {
            printf("\nHere are your options:\n");
            printf("  - Adjust render step (1)\n");
            printf("  - Adjust zoom level (2)\n");
            printf("  - Change coordinate plane (3)\n");
            printf("  - Change walkthrough settings (4)\n");
            printf("  - Return to previous menu (-1)\n");

            scanf("%d", &user_choice);

            if (user_choice == 1)
            {
                printf("\nRender step refers to how often images are displayed\n");
                printf("The current render step is %s meaning that images are displayed at intervals of %s", display_time(render_step), display_time(render_step));
                printf("\nWhat do you want the render step to be? Enter in the format: days hours minutes (e.g., 7 0 0):\n");
                scanf("%d %d %d", &days, &hours, &minutes);
                time_seconds = (days * DAY) + (hours * HOUR) + (minutes * MINUTE);

                render_step = time_seconds;

                printf("\nRender step reassigned successfully! Render step is %s\n", display_time(render_step));
            }
            else if (user_choice == 2)
            {
                printf("\nZoom level refers to how zoomed in the images are rendered\n");
                printf("The current zoom level is: %d", zoom);
                printf("\nWhat do you want the zoom level to be?\n");
                scanf("%d", &zoom);

                printf("\nZoom level reassigned successfully! Zoom step is %d\n", zoom);
                
            }
            else if (user_choice == 3)
            {
                char *plane_str;
                if (plane == XY)
                {
                    plane_str = "XY";
                }
                else if (plane == YZ)
                {
                    plane_str = "YZ";
                }
                else
                {
                    plane_str = "XZ";
                }



                printf("\nCoordinate plane refers to what two axes make the rendered image\n");
                printf("The current coordinate plane is: %s", plane_str);
                printf("\nWhat do you want the coordinate plane to be? XY(0), YZ(1) or XZ(2)\n");
                scanf("%d", &plane);

                printf("\nCoordinate plane changed successfully!\n");
            }
            else if (user_choice == 4)
            {
                printf("\nWalkthrough refers to if you want for each image to stay on the screen until dismissed\n");
                printf("\nThe current walkthrough setting is: %d", render_walkthrough);
                printf("\nWhat do you want the walkthrough setting to be? True(1) or false(0)\n");
                scanf("%d", &render_walkthrough);

                printf("\nWalkthrough setting changed successfully!\n");
            }
            
            user_choice = 0;
        }

    } while (user_choice != -1);
    
    menu_banner(0);

    return 0;
}


void simulate(Object *sim_log, Object objects[], int time_seconds)
{
    // i timestep = delta_time
    for (int i = 0; i < (time_seconds / delta_time) + 1; i++)
    {

        // log every log_step
        update_log(sim_log, objects, i * delta_time);
        
        apply_gravitational_forces_N(objects);
        update_N(objects);
    }

}


void menu_banner(int menu)
{   
    int border_length = 50;
    char *string;

    printf("\n\n\n");

    if (menu == 0)
    {
        string = "  | Main Menu |";
    }
    else if (menu == 1)
    {
        string = "| Simulation Menu |";
    }
    else if (menu == 2)
    {
        string = " | Settings Menu |";
    }

    for (int i = 0; i < border_length; i++)
    {
        printf("-");
    }

    printf("\n\t\t%s\n", string);

    for (int i = 0; i < border_length; i++)
    {
        printf("-");
    }
}


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


