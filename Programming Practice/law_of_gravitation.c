#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define MINUTE 60
#define HOUR MINUTE * 60
#define DAY HOUR * 24
#define WEEK DAY * 7

// distance from middle of render to the edge of render in metres
#define RENDER_SIZE 400000000 // 400 million

// pixel ratio for square grid
#define NO_PIXELSX 17 // 17
#define NO_PIXELSY 21 // 21

#define NO_OBJECTS 2

const double GRAVITATIONAL_CONSTANT = 6.67430e-11;

// update every minute
const double DELTA_TIME = MINUTE;

// enum for plane axes
enum Planes {XY, YZ, ZX};

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
void update_N(Object[]);

void render_objects(Object[], int ,float);

int main()
{
    Object objects[NO_OBJECTS];
    
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

    // test object
    objects[2].mass = 7.348e25;  // kg
    objects[2].motion.position = (Vec3){-384400000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[2].motion.velocity = (Vec3){0.0f, -1022.0f, 0.0f};        // m/s (orbital speed)
    objects[2].motion.force = (Vec3){0.0f, 0.0f, 0.0f};        // m/s (orbital speed)
    
    for (int i = 0; i < ((WEEK*4) / DELTA_TIME); i++)
    {

        // render every day
        if (i % (DAY / (int)DELTA_TIME) == 0)
        {
            printf("\nDay %d\n", (i / (DAY / (int)DELTA_TIME)));
            render_objects(objects, XY, 1);
        }
        
        apply_gravitational_forces_N(objects);
        update_N(objects);
    }

    return 0;
}

double distance(Object object1, Object object2)
{
    double distanceX = object1.motion.position.x - object2.motion.position.x;
    double distanceY = object1.motion.position.y - object2.motion.position.y;
    double distanceZ = object1.motion.position.z - object2.motion.position.z;

    return sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
};

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

void update(Object *object) {
    Vec3 acceleration = {
        object->motion.force.x / object->mass,
        object->motion.force.y / object->mass,
        object->motion.force.z / object->mass
    };

    object->motion.velocity.x += acceleration.x * DELTA_TIME;
    object->motion.velocity.y += acceleration.y * DELTA_TIME;
    object->motion.velocity.z += acceleration.z * DELTA_TIME;

    object->motion.position.x += object->motion.velocity.x * DELTA_TIME;
    object->motion.position.y += object->motion.velocity.y * DELTA_TIME;
    object->motion.position.z += object->motion.velocity.z * DELTA_TIME;
}

void update_N(Object objects[])
{
    for (int i = 0; i < NO_OBJECTS; i++)
    {
        update(&objects[i]);
    }
}


void display_position(Object object)
{
    printf("\nx-coordinate: %e", object.motion.position.x);
    printf("\ny-coordinate: %e", object.motion.position.y);
    printf("\nz-coordinate: %e\n", object.motion.position.z);

};


void render_objects(Object objects[], int plane, float zoom)
{
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
            coordinates[i].x = (objects[i].motion.position.x / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.y / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == YZ)
        {
            coordinates[i].x = (objects[i].motion.position.y / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.z / pixel_sizeY) + (half_screen_sizeY);
        }
        else if (plane == ZX)
        {
            coordinates[i].x = (objects[i].motion.position.z / pixel_sizeX) + (half_screen_sizeX);
            coordinates[i].y = (objects[i].motion.position.x / pixel_sizeY) + (half_screen_sizeY);
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


}