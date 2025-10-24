#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define MINUTE 60
#define HOUR MINUTE * 60
#define DAY HOUR * 24
#define WEEK DAY * 7

const double GRAVITATIONAL_CONSTANT = 6.67430e-11;

// every minute
const double DELTA_TIME = MINUTE;

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
} Motion;

typedef struct
{
    double mass;
    Motion motion;

} Object;

// calculates distance between two objects
double distance(Object, Object);

double gravitational_force(Object, Object);
Vec3 gravitational_force_vector(Object, Object);

// updates an object's velocity based on the forces acting on it
void update_velocity(Object*, Vec3);

// updates an object's position based on it's velocity
void update_position(Object*);
void display_position(Object);

void update(Object *object, Vec3 force);

int main()
{
    Object objects[2];
    Vec3 force1;
    Vec3 force2;
    
    // Earth
    objects[0].mass = 5.972e24;  // kg
    objects[0].motion.position = (Vec3){0.0f, 0.0f, 0.0f};
    objects[0].motion.velocity = (Vec3){0.0f, 0.0f, 0.0f};

    // Moon
    objects[1].mass = 7.348e22;  // kg
    objects[1].motion.position = (Vec3){384400000.0f, 0.0f, 0.0f};  // meters from Earth
    objects[1].motion.velocity = (Vec3){0.0f, 1022.0f, 0.0f};        // m/s (orbital speed)

    printf("The distance between object 1 and object 2 is: %f\n", distance(objects[0], objects[1]));
    printf("The gravitation force between object 1 and object 2 is: %e", gravitational_force(objects[0], objects[1]));

    Vec3 force = gravitational_force_vector(objects[0], objects[1]);

    printf("\n\nThe gravitation force between object 1 and object 2 split along each axis is:");
    
    printf("\nx-axis: %e", force.x);
    printf("\ny-axis: %e", force.y);
    printf("\nz-axis: %e", force.z);
    

    
    force = gravitational_force_vector(objects[1], objects[0]);

    printf("\n\nThe gravitation force between object 1 and object 2 split along each axis is:");
    
    printf("\nx-axis: %e", force.x);
    printf("\ny-axis: %e", force.y);
    printf("\nz-axis: %e", force.z);


 for (int i = 0; i < ((WEEK*4) / DELTA_TIME); i++)
    {

        // display results every hour
        if (i % 60 == 0)
        {
            printf("\nDay %d, Hour %d:", (i / 60) /24, (i / 60) % 24);
            display_position(objects[1]);
        }

        force1 = gravitational_force_vector(objects[0], objects[1]);
        force2 = gravitational_force_vector(objects[1], objects[0]);

        update(&objects[0], force1);
        update(&objects[1], force2);
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

double gravitational_force(Object object1, Object object2)
{
    double r = distance(object1, object2);
    return (GRAVITATIONAL_CONSTANT * (object1.mass * object2.mass)) / (r * r);
};

Vec3 gravitational_force_vector(Object object1, Object object2) {
    Vec3 r;
    r.x = object2.motion.position.x - object1.motion.position.x;
    r.y = object2.motion.position.y - object1.motion.position.y;
    r.z = object2.motion.position.z - object1.motion.position.z;

    double distance = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
    double force_magnitude = (GRAVITATIONAL_CONSTANT * object1.mass * object2.mass) / (distance * distance);

    Vec3 force;
    force.x = force_magnitude * r.x / distance;
    force.y = force_magnitude * r.y / distance;
    force.z = force_magnitude * r.z / distance;

    return force;
}

void update(Object *object, Vec3 force) {
    Vec3 acceleration = {
        force.x / object->mass,
        force.y / object->mass,
        force.z / object->mass
    };
    object->motion.velocity.x += acceleration.x * DELTA_TIME;
    object->motion.velocity.y += acceleration.y * DELTA_TIME;
    object->motion.velocity.z += acceleration.z * DELTA_TIME;

    object->motion.position.x += object->motion.velocity.x * DELTA_TIME;
    object->motion.position.y += object->motion.velocity.y * DELTA_TIME;
    object->motion.position.z += object->motion.velocity.z * DELTA_TIME;
}




void display_position(Object object)
{
    printf("\nx-coordinate: %e", object.motion.position.x);
    printf("\ny-coordinate: %e", object.motion.position.y);
    printf("\nz-coordinate: %e\n", object.motion.position.z);

};