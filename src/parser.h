/**
 * Filename: parser.cpp
 *
 * Author: Glenn Skelton
 * Last Modified: April 1, 2019
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "givr.h"
#include "glm/gtc/matrix_transform.hpp"
#include "boid.h"

using namespace std;
using namespace givr;


// struct for storing program specific parameters read in from parser and used throughout
struct ProgramParameters {
    unsigned int numBoids; // total number of boids
    float boidMass; // the total mass of the boid
    float minVelocity; // boid minimum velocity to travel
    float maxVelocity; // boid maximum velocity to travel

    float avoidanceRange; // avoidance/cohesion barrier
    float avoidanceMultiplier; // value to multiple the avoidance force by

    float cohesionRange; // cohesion/gather barrier
    float cohesionMultiplier; // value to multiply the cohesion force by

    float maxSearchRange; // max search radius
    float gatherMultiplier; // value to multiply the gather force by

    float arenaRadius; // radius of the arena to contain boids

    float forceMultiplier; // value to multiply force by

    vector<Boid*> *boids = new vector<Boid*>(); // list storing all of the boids
    vector<float> *graphValues = new vector<float>(); // list storing the graph data

    int boidFunc; // index value
};




namespace givr {
namespace fileIO {

    bool parseConfigFile(struct ProgramParameters &p,
                         const string &filename);
    bool saveConfigFile(struct ProgramParameters &p,
                        const string &filename = "config.txt");

} // namespace io
} // namespace givr
