#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <windows.h>


// time units in seconds
#define MINUTE (60)
#define HOUR (MINUTE * 60)
#define DAY (HOUR * 24)
#define WEEK (DAY * 7)


// simulation constants
#define NO_OBJECTS 4
const double GRAVITATIONAL_CONSTANT = 6.67430e-11;


// simulation configuration
int delta_time = MINUTE; // simulaton step duration
int log_step = MINUTE; // how often data is recorded
int time_scale = (WEEK * 4); // total duration of the simulation


// derived intervals
#define MINUTE_INTERVAL (MINUTE / delta_time)
#define HOUR_INTERVAL ( HOUR / delta_time )
#define DAY_INTERVAL ( DAY / delta_time )
#define WEEK_INTERVAL ( WEEK / delta_time ) 


// enum for plane axes
enum Planes {XY, YZ, XZ};


// render configuration
#define RENDER_SIZE 400000000 // 400 million metres (half-width of view)
// NO_PIXELSX / NO_PIXELSY = 0.81 for square grid
#define NO_PIXELSX 34 // 17
#define NO_PIXELSY 42 // 21
int render_step = DAY; // how often rendering occurs
bool render_wait = true; // pause after each render
int zoom = 1; // render zoom level
double offsetX = 0;
double offsetY = 0;
int plane = XY; // 


typedef struct
{
    double x, y, z;
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


// core physics
double distance(Object, Object);
void apply_gravitational_forces(Object*, Object*);
void apply_gravitational_forces_N(Object[]);


// state updates
void update(Object *object);
void update_N(Object[]);


// simulation log
void update_log(Object*, Object[], int time);
Object* get_log_data(Object *sim_log, int time_seconds);


// simulation control
void simulate(Object *sim_log, Object initial_objects[] ,Object objects[], int time_seconds);


// rendering
void render_objects(Object[], int time_seconds, int plane, float zoom);
void render_objects_advanced(Object *sim_log, int time_seconds ,int plane, float zoom, int start, int end);
void render_objects_over_time(Object *sim_log, int plane, float zoom, int start, int end);


// utility
bool is_interval(int, int);
char* display_time(int);
void display_position(Object);
void display_all_information(Object objects[]);
void clear_input_buffer();


// ui
int program_ui(Object *sim_log, Object[] ,Object[]);
int simulation_ui(Object *sim_log, Object[], Object[]);
int settings_ui();
void intro();
void menu_banner(int menu);



int main()
{
    Object objects[NO_OBJECTS];
    Object initial_objects[NO_OBJECTS];
    
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
    objects[0].motion.velocity = (Vec3){0.0f, 30000.0f, 0.0f};
    objects[0].motion.force = (Vec3){0.0f, 0.0f, 0.0f};
    objects[0].symbol = 'E';

    // Moon
    objects[1].mass = 7.348e22;  // kg
    objects[1].motion.position = (Vec3){384400000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[1].motion.velocity = (Vec3){0.0f, 30000.0f + 1022.0f, 0.0f};        // m/s (orbital speed)
    objects[1].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    // moon orbital speed 1022.0f
    objects[1].symbol = 'M';

    // Satellite
    objects[2].mass = 6000;  // kg
    objects[2].motion.position = (Vec3){36000000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[2].motion.velocity = (Vec3){0.0f, 30000.0f + 2000.0f, 2000.0f};        // m/s (orbital speed)
    objects[2].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    objects[2].symbol = 'S';

    // Sun
    objects[3].mass = 1.989e30;  // kg
    objects[3].motion.position = (Vec3){-150000000000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[3].motion.velocity = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    objects[3].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    objects[3].symbol = 'o';

    memcpy(initial_objects, objects, sizeof(objects));

    // set initial values
    simulate(simulation_log, initial_objects, objects, time_scale);
    program_ui(simulation_log, initial_objects, objects);
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

/*
    core physics
*/
//calculates the distance between two objects
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




/*
    state updates
*/
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




/*
    simulation log
*/
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


// retrieves log data
Object* get_log_data(Object *sim_log, int time_seconds)
{
    int index = (time_seconds / log_step);

    return &sim_log[index * NO_OBJECTS];
}




/*
    simulation control
*/
void simulate(Object *sim_log, Object initial_objects[] , Object objects[], int time_seconds)
{
    memcpy(objects, initial_objects, NO_OBJECTS * sizeof(objects[0]));

    // i timestep = delta_time
    for (int i = 0; i < (time_seconds / delta_time) + 1; i++)
    {

        // log every log_step
        update_log(sim_log, objects, i * delta_time);
        
        apply_gravitational_forces_N(objects);
        update_N(objects);
    }

    display_all_information(objects);
}




/*
    rendering
*/
// renders all the objects in ASCII in a given area
void render_objects(Object objects[], int time_seconds ,int plane, float zoom)
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
            plane_str = "|   VIEW: X <> | Y v^ |";
            coordinates[i].x = ((objects[i].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = ((objects[i].motion.position.y + offsetY) / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == YZ)
        {
            plane_str = "|   VIEW: Y <> | Z v^ |";
            coordinates[i].x = ((objects[i].motion.position.y + offsetX) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = ((objects[i].motion.position.z + offsetY) / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == XZ)
        {
            plane_str = "|   VIEW: X <> | Z v^ |";
            coordinates[i].x = ((objects[i].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = ((objects[i].motion.position.z + offsetY) / pixel_sizeY) + (half_screen_sizeY);
        }
    }


    printf("\n\n%s", display_time(time_seconds));
    printf("   ZOOM: \033[36m%4.2fx\033[0m   ", zoom);
    printf("%s\n", plane_str);
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
    
}


void render_objects_advanced(Object *sim_log, int time_seconds ,int plane, float zoom, int start, int end)
{
    char *plane_str;
    Vec3 coordinates[NO_OBJECTS];
    int trail[NO_PIXELSX][NO_PIXELSY];
    char slope_position[NO_PIXELSX][NO_PIXELSY];

    for (int x = 0; x < NO_PIXELSX; x++)
    {
        for (int y = 0; y < NO_PIXELSY; y++)
        {
            trail[x][y] = 0;
        }
    }

    bool displayed = false;

    // number of pixels from the middle to the end
    int half_screen_sizeX = NO_PIXELSX / 2;
    int half_screen_sizeY = NO_PIXELSY / 2;

    double pixel_sizeX = ((double)RENDER_SIZE / half_screen_sizeX) / zoom;
    double pixel_sizeY = ((double)RENDER_SIZE / half_screen_sizeY) / zoom;

    for (int i = 0; i < NO_OBJECTS; i++)
    {
        if (plane == XY)
        {
            plane_str = "|   VIEW: X <> | Y v^ |";
            coordinates[i].x = (int)((get_log_data(sim_log, time_seconds)[i].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (int)((get_log_data(sim_log, time_seconds)[i].motion.position.y + offsetY) / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == YZ)
        {
            plane_str = "|   VIEW: Y <> | Z v^ |";
            coordinates[i].x = ((get_log_data(sim_log, time_seconds)[i].motion.position.y + offsetY) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = ((get_log_data(sim_log, time_seconds)[i].motion.position.z) / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == XZ)
        {
            plane_str = "|   VIEW: X <> | Z v^ |";
            coordinates[i].x = ((get_log_data(sim_log, time_seconds)[i].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = ((get_log_data(sim_log, time_seconds)[i].motion.position.z) / pixel_sizeY) + (half_screen_sizeY);
        }
    }


    printf("\n\n%s", display_time(time_seconds));
    printf("   ZOOM: \033[36m%4.3fx\033[0m   ", zoom);
    printf("%s\n", plane_str);

    int trailx;
    int traily;
    float ratio;
    double vx;
    double vy;

    
    for (int i = start / log_step; i < (end / log_step); i++)
    {
        // movement relative to earth
        offsetX = -1 * get_log_data(sim_log, i * log_step)[0].motion.position.x;
        offsetY = -1 * get_log_data(sim_log, i * log_step)[0].motion.position.y;

        for (int j = 0; j < NO_OBJECTS; j++)
        {
            if (plane == XY)
            {
                trailx = (int)((get_log_data(sim_log, i * log_step)[j].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
                traily = (int)(NO_PIXELSY - 1) - (int)(((get_log_data(sim_log, i * log_step)[j].motion.position.y + offsetY) / pixel_sizeY) + (half_screen_sizeY));
            }
            else if (plane == YZ)
            {
                trailx = ((get_log_data(sim_log, i * log_step)[j].motion.position.y + offsetY) / pixel_sizeX) + (half_screen_sizeX);
                traily = (NO_PIXELSY - 1) - (int)(((get_log_data(sim_log, i * log_step)[j].motion.position.z) / pixel_sizeY) + (half_screen_sizeY));

            }
            else if (plane == XZ)
            {
                trailx = ((get_log_data(sim_log, i * log_step)[j].motion.position.x + offsetX) / pixel_sizeX) + (half_screen_sizeX);
                traily = (NO_PIXELSY - 1) - (int)(((get_log_data(sim_log, i * log_step)[j].motion.position.z) / pixel_sizeY) + (half_screen_sizeY));
            }

        
            if (trailx >= 0 && trailx < NO_PIXELSX && traily >= 0 && traily < NO_PIXELSY)
            {
                vx = get_log_data(sim_log, i * log_step)[j].motion.velocity.x;
                vy = get_log_data(sim_log, i * log_step)[j].motion.velocity.y;
                
                if (fabs(vx) < 1e-6)
                    vx = 1e-6; // avoid division by zero
                ratio = vy / (vx + 1.0);

                if (ratio > 4.0) {
                    slope_position[trailx][traily] = '|';   // steep upward
                } else if (ratio > 0.5) {
                    slope_position[trailx][traily] = '/';   // moderate upward
                } else if (ratio > -0.5) {
                    slope_position[trailx][traily] = '=';   // mostly horizontal
                } else if (ratio > -4.0) {
                    slope_position[trailx][traily] = '\\';  // moderate downward
                } else {
                    slope_position[trailx][traily] = '|';   // steep downward
                }

                trail[trailx][traily] = 1;
            }

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
                    printf(" \033[32m%c\033[0m ", get_log_data(sim_log, 0)[ob].symbol);
                    displayed = true;
                    break;

                }
                
            }
            
            if (!displayed && trail[x][y] == 1)
            {
                printf("\033[34m %c \033[0m", slope_position[x][y]);
                displayed = true;
            }

            if (!displayed)
            {
                printf(" . ");
            }

            displayed = false;

        }
        
        printf("\n");
    }
    
}


void render_objects_over_time(Object *sim_log, int plane, float zoom, int start, int end)
{
    int time_seconds;
    int i = (start / render_step);
    char input;
    clear_input_buffer();

    bool go_back = false;
    bool paused = false;

    while (i < (end / render_step) + 1)
    {
        time_seconds = i * render_step;
        //render_objects(get_log_data(sim_log, time_seconds), time_seconds, plane, zoom);

        // view focused on earth
        offsetX = -1 * get_log_data(sim_log, time_seconds)[0].motion.position.x;
        offsetY = -1 * get_log_data(sim_log, time_seconds)[0].motion.position.y;


        render_objects_advanced(sim_log, time_seconds, plane, zoom, start, end);
        printf("[ ENTER > ] [ b < ]   [ +ZOOM ] [ -ZOOM ]   [ 0 XY ] [ 1 YZ ] [ 2 XZ ]   [ QUIT ]");

        if (render_wait)
        {
            input = getchar();

            switch (input)
            {
            case 'b':
                go_back = true;
                clear_input_buffer();
                break;

            case '+':
                paused = true;
                zoom *= 2;
                clear_input_buffer();
                break;

            case '-':
                paused = true;
                zoom /= 2;
                clear_input_buffer();
                break;

            case '0':
            case '1':
            case '2':
                paused = true;
                plane = input - '0';
                clear_input_buffer();
                break;

            case 'q':
                return;
    
            default:
                break;
            }

        }
        
        if(go_back)
        {
            i--;
            go_back = false;
        }
        else if (!paused)
        {
            i++;
        }
        else
        {
            paused = false;
        }

    }
}




/*
    utility
*/
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
    snprintf(time_str, sizeof(time_str), "| TIME: DAY \033[36m%2d\033[0m | HOUR \033[36m%2d\033[0m | MINUTE \033[36m%2d\033[0m", days, hours, minutes);
    strcat(time_str, "\033[0m |");
    return time_str;
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


// returns true if step is at a given time interval
bool is_interval(int interval, int step)
{
    if (interval == 0)
    {
        return false;
    }

    return step % interval == 0;
}


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}




/*
    ui
*/
int program_ui(Object *sim_log, Object initial_objects[], Object objects[])
{
    intro();
    printf("This should not be red \033[31mThis text is red!\033[0m This should also not be red\n");
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
            simulation_ui(sim_log, initial_objects, objects);
        }
        else if (user_choice == 2)
        {
            settings_ui();
        }

    } while (user_choice != -1);
    
    return 0;
}


int simulation_ui(Object *sim_log, Object initial_objects[], Object objects[])
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
            //render_objects(&sim_log[0 * NO_OBJECTS],0, XY, 1);
            render_objects_advanced(sim_log, 0, plane, zoom, 0, 0);
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
            simulate(sim_log, initial_objects ,objects, time_seconds);
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
                printf("\nThe current walkthrough setting is: %d", render_wait);
                printf("\nWhat do you want the walkthrough setting to be? True(1) or false(0)\n");
                scanf("%d", &render_wait);

                printf("\nWalkthrough setting changed successfully!\n");
            }
            
            user_choice = 0;
        }

    } while (user_choice != -1);
    
    menu_banner(0);

    return 0;
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

