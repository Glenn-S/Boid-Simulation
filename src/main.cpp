//------------------------------------------------------------------------------
// Filename: main.cpp
//
// Assignment 4
// Author: Glenn Skelton
//
// Last modified: April 10, 2019
//
// random number generator source:
// https://stackoverflow.com/questions/686353/random-float-number-generation
//
// bee model:
// https://free3d.com/3d-model/bee-89226.html
//------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include "io.h"
#include "panel.h"
#include "turntable_controls.h"
#include "boid.h"
#include "parser.h"
#include <ctime>
#include <cstdlib>
///////////////////////////////////////////////////////////////////////////////////////////////////
using namespace glm;
using namespace givr;
using namespace givr::camera;
using namespace givr::geometry;
using namespace givr::style;
using namespace givr::fileIO;
///////////////////////////////////////////////////////////////////////////////////////////////////

// GLOBAL VARIABLES
struct ProgramParameters params;


// GLOBAL CONSTANTS/STATE
constexpr float DELTA_T = 0.001;   // seconds (time step) 0.05
constexpr unsigned int INTEGRATION = 16; // number of integrations
const vec3f GRAVITY(0.0, 9.81, 0.0);

bool PAUSED = false;
bool OBSTACLE_MODE = false;


// FUNCTION DEFINITIONS
float randomFloatGenerator(const float &lower, const float &upper);


///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Purpose: Create a scene that contains birds that are flocking together.
 *
 * @brief main
 * @return
 */
int main(void) {
    namespace p = panel; // use p in place of panel
    p::clear_color = ImVec4(0.0, 0.0, 0.0, 1.0);

    io::GLFWContext windows;
    auto window = windows.create(io::Window::dimensions{800, 600}, "CPSC 587 Assignment 4"); // generate window instance
    window.enableVsync(true);

    io::ImGuiContext ImGui(window); // create mGUI context for graph editor

    auto view = View(TurnTable(), Perspective()); // create view matrix
    view.camera.zoom(100.0f); // zoom camera out
    TurnTableControls controls(window, view.camera); // initialize controls for window

    srand(static_cast<unsigned>(time(0))); // initialize the random number generator with a seed




    /////////////////////////////////// READ CONTEXT FILE ////////////////////////////////////////
    if (parseConfigFile(params, "configFiles/config.txt")) {

        // generate boids with given information
        if (params.numBoids > 0) {
            for (unsigned int i = 0; i < params.numBoids; i++) {
                // randomly assign a boid with a position and starting velocity of min
                vec3f startPos = vec3f(randomFloatGenerator(-1.0, 1.0) * params.arenaRadius,
                                       randomFloatGenerator(-1.0, 1.0) * params.arenaRadius,
                                       randomFloatGenerator(-1.0, 1.0) * params.arenaRadius);
                vec3f startVel = vec3f(randomFloatGenerator(-1.0, 1.0) * params.minVelocity,
                                       randomFloatGenerator(-1.0, 1.0) * params.minVelocity,
                                       randomFloatGenerator(-1.0, 1.0) * params.minVelocity);
                params.boids->push_back(new Boid(i,
                                                 params.boidMass,
                                                 startPos,
                                                 startVel,
                                                 vec3f(0, 0, 0)));
            }
        }

        // if true, parse into the memoized function and boids info
        if (params.graphValues->size() != 0) {
            params.boidFunc = p::funcs.create("y Value", params.graphValues->size()); // number of buckets for y value
            p::funcs.curvesData().at(params.boidFunc).memoized = io::MemoizeFunction(*params.graphValues);

            // update the curve to have the points read in
            float incrementVal = 1.0 / params.graphValues->size();
            float xVal = 0.0f;
            vector<ImVec2> curveValues;
            for (float p : *params.graphValues) {
                curveValues.push_back(ImVec2(xVal, p));
                xVal += incrementVal;
            }
            p::funcs.curvesData().at(params.boidFunc).curve.load(curveValues);
        }
    } else {
        // use default values
        params.boidFunc = p::funcs.create("Boid Function Value", 90); // number of buckets for y value
    }


    //////////////////////////////////// CREATE GEOMETRY /////////////////////////////////////////

    auto instancedBee = createInstancedRenderable(Mesh(Filename("../../models/bee.obj")),
                                                  Phong(Colour(1., 1., 0.1529), LightPosition(100.f, 100.f, 100.f)));



    ///////////////////////////////////// CREATE OBSTACLES ///////////////////////////////////////
    // create a cylinder to fly around
    vector<CylinderGeometry> *obstacles = new vector<CylinderGeometry>();

    obstacles->push_back(Cylinder(Point1(0.0, 0.0, params.arenaRadius),
                                  Point2(0.0, 0.0, -params.arenaRadius),
                                  Radius(params.maxSearchRange - (params.maxSearchRange * 0.3))));

    auto cylinder = createRenderable(obstacles->at(0),
                                     Phong(Colour(1.0, 0.0, 0.0), LightPosition(100.f, 100.f, 100.f)));


    //////////////////////////////// KEYBOARD CALLBACKS /////////////////////////////////////////
    window.keyboardCommands() |
        io::Key(GLFW_KEY_P, [](io::KeyboardEvent key) {
            if (key.action == GLFW_RELEASE)
                p::showPanel = !p::showPanel; // show panel
        }) |
        io::Key(GLFW_KEY_S, [](io::KeyboardEvent key) { // ctrl-s
            // save the current graph information
            if ( (key.action == GLFW_RELEASE) && (key.mods == GLFW_MOD_CONTROL) ) {
                params.graphValues->clear();
                const float *datum = p::funcs.curvesData().at(params.boidFunc).memoized.data();
                for (signed int i = 0; i < p::funcs.curvesData().at(params.boidFunc).memoized.bucketCount(); i++)
                    params.graphValues->push_back(datum[i]);

                if (saveConfigFile(params, "configFiles/config.txt")) cout << "file succesfully saved" << endl;
                else cout << "file write failed" << endl;
            }
        }) |
        // pause simulation
        io::Key(GLFW_KEY_SPACE, [](io::KeyboardEvent key) {
            if (key.action == GLFW_RELEASE) {
                PAUSED = !PAUSED;
            }
        }) |
        // turn on obstacle mode
        io::Key(GLFW_KEY_1, [](io::KeyboardEvent key) {
            if (key.action == GLFW_RELEASE) {
                OBSTACLE_MODE = !OBSTACLE_MODE;
                if (OBSTACLE_MODE) cout << "Obstacle Mode Engaged" << endl;
            }
        });



    //----------------------------------------------------------------------------------------------
    // GRAPHICS LOOP
    //----------------------------------------------------------------------------------------------
    window.run([&](float) {
        glfwPollEvents(); // wait for interrupts
        p::menu(); // instatiate menu

        auto color = p::clear_color; // menu colour clear
        glClearColor(color.x, color.y, color.z, color.w); // screen clear color

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
        view.projection.updateAspectRatio(window.width(), window.height()); // update the view matrix


        if (!PAUSED) {
            for (unsigned int i = 0; i < INTEGRATION; i++) { // integrate multiple times



                // go through each boid and calculate personal forces
                for (Boid *b : *params.boids) {
                    b->calculateBoundaryForce(params.arenaRadius, params.forceMultiplier);

                    if (OBSTACLE_MODE) {
                        // test for object collisions
                        for (CylinderGeometry o : *obstacles) {
                            vec3f cylinderOrigin = (o.p1() + o.p2()) * 0.5f; // get the midway vector

                            vec3f nextPos = b->getPosition() + b->getVelocity() * DELTA_T; // look ahead
                            vec3f currVel = b->getVelocity();
                            vec2f testPos = vec2f(nextPos.x, nextPos.y) -
                                            vec2f(cylinderOrigin.x, cylinderOrigin.y);
                            vec2f testVel = vec2f(currVel.x, currVel.y);
                            float r = o.radius() + (params.maxSearchRange * 0.3f);

                            float A = dot(testVel, testVel);
                            float B = 2 * dot(testVel, testPos);
                            float C = dot(testPos, testPos) - (r * r);

                            float descriminant = (B * B) - (4 * A * C);
                            float denominator = 0.0f;
                            float t = 0.0f, t1 = 0.0f, t2 = 0.0f;

                            if (descriminant >= 0) { // one or more solutions
                                if (descriminant == 0) {
                                    t = -B / (2 * A);
                                } else {
                                    descriminant = sqrt(descriminant);
                                    denominator = -B - descriminant;
                                    if (denominator != 0) t1 = denominator / (2 * A);
                                    denominator = -B + descriminant;
                                    if (denominator != 0) t2 = denominator / (2 * A);

                                    // take the larger of the smaller of the two
                                    t = t1 < t2 ? t1 : t2;
                                }
                                // if collision detected, calculate force to apply to the boid
                                if (t > 0.0) {
                                    // plug back into x + tv to give point in 3d where it intersects
                                    vec3f intersect = b->getPosition() + t * b->getVelocity(); // intersect point

                                    // calculate normal and tangential force
                                    vec3f resultant = vec3f(0.0f, 0.0f, 0.0f);
                                    vec3f normal = vec3f(intersect.x, intersect.y, 0.0f) - vec3f(cylinderOrigin.x, cylinderOrigin.y, 0.0f);
                                    normal = glm::normalize(normal);
                                    resultant += normal * params.forceMultiplier;

                                    vec3f tangent = b->getVelocity() - (glm::dot(b->getVelocity(), normal) * normal);
                                    tangent = glm::normalize(tangent);
                                    resultant += tangent * params.forceMultiplier;

                                    b->addNetForce(resultant);
                                }
                            }
                        }
                    }





                    // calculate boid to boid interactions
                    for (Boid *o_b : *params.boids) // N^2 version
                        if (b != o_b)
                            if (o_b->getID() < b->getID()) {
                                vec3f force(0, 0, 0);
                                vec3f direction = o_b->getPosition() - b->getPosition();
                                float dist = glm::length(direction);
                                float evalResult = 0.0;
                                direction = glm::normalize(direction);

                                float avoid = params.avoidanceRange;
                                float cohesion = params.cohesionRange;
                                float max = params.maxSearchRange;
                                float ratio = 0.0f;

                                // boid / boid testing
                                if (dist < avoid) { // withing avoidance range
                                    ratio = (dist / avoid) * 0.333;
                                    evalResult = p::funcs.evaluateFast(params.boidFunc, ratio);
                                    force = evalResult * -direction * params.avoidanceMultiplier;
                                } else if (dist < cohesion) { // withing cohesion range
                                    ratio = (dist / cohesion) * 0.666;
                                    evalResult = p::funcs.evaluateFast(params.boidFunc, ratio);
                                    force = evalResult * (o_b->getVelocity() - b->getVelocity()) * params.cohesionMultiplier;
                                } else if (dist < params.maxSearchRange) { // within gather range
                                    ratio = (dist / max) * 1.0f;
                                    evalResult = p::funcs.evaluateFast(params.boidFunc, ratio);
                                    force = evalResult * direction * params.gatherMultiplier;
                                } else { // at max range or greater
                                    // ignore
                                }

                                // apply forces to each boid respectively
                                b->addNetForce(force);
                                o_b->addNetForce(-force);
                            }
                }

                // go through each boid and update positions
                for (Boid *b : *params.boids)
                    b->updateBoidPosition(DELTA_T, params.minVelocity, params.maxVelocity);

            }


        }

        // calculate the orientation of the boid
        for (Boid *b : *params.boids) {
            vec3f T = glm::normalize(b->getVelocity()); // tangent vector
            vec3f B = glm::normalize(glm::cross(glm::normalize(GRAVITY + b->getLastForce()), T));
            vec3f N = glm::normalize(glm::cross(B, T));
            B = normalize(glm::cross(T, N)); // make orthonormal
            vec3f p = b->getPosition();

            mat4f model = {{B.x, B.y, B.z, 0.0},
                           {N.x, N.y, N.z, 0.0},
                           {T.x, T.y, T.z, 0.0},
                           {p.x, p.y, p.z, 1.0}};
            addInstance(instancedBee, model);
        }


        // RENDER
        draw(instancedBee, view); // send data to GPU
        // create the obstacle
        if (OBSTACLE_MODE) draw(cylinder, view);

        io::renderDrawData(); // needed for rendering the panel

    });


    // reclaim memory
    for (Boid *b : *params.boids)
        delete b;
    params.boids->clear();
    delete params.boids;
    params.graphValues->clear();
    delete params.graphValues;

    exit(EXIT_SUCCESS);
}

//////////////////////////////////////////////////////////////////////////////////////////
// ADDITIONAL FUNCTIONS

float randomFloatGenerator(const float &lower, const float &upper) {
    return ( static_cast<float>(rand()) / static_cast<float>(RAND_MAX/(upper - lower)) ) + lower;
}





//////////////////////////////////////////////////////////////////////////////////////////

