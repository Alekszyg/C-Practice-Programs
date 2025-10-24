#include <stdio.h>
#include <math.h>
#include <stdbool.h>

const double GRAVITATIONAL_CONSTANT = 6.67430e-11;

typedef struct
{
    float x;
    float y;
    float z;

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


int main()
{
    float tick = 1;
    Object objects[2];

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
    while (tick < 100)
    {
        tick++;
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