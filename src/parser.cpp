/**
 * Filename: parser.cpp
 *
 * Author: Glenn Skelton
 * Last Modified: April 1, 2019
 */

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <exception>
#include "givr.h"
#include "glm/gtc/matrix_transform.hpp"
#include "boid.h"
#include "parser.h"

using namespace std;
using namespace givr;


/**
 * To read in a configuration file and populate the state information based
 * on the values read in.
 */
bool givr::fileIO::parseConfigFile(struct ProgramParameters &p,
                                   const string &filename) { // will get graph information and boid start state

    int readValue = 0;
    signed int defaultBoids = 5;
    signed int dataPoints = 0;
    float dataValue = 0.0f;



    // open the file for read and write
    string line;

    try {
        ifstream iFile("../../" + filename);

        if (iFile.is_open()) {

            // read in the file
            while (getline(iFile, line)) {
                if ((char)line[0] != '#') { // not in comment

                    // NUMBER OF BOIDS
                    if (strncmp(line.c_str(), "boids: ", 7) == 0) { // read in number of boids
                        readValue = sscanf(line.c_str(), "boids: %d", &p.numBoids);
                        if (readValue != 1) {
                            cout << "error reading in boids count" << endl;
                            p.numBoids = defaultBoids; // default value
                        } else {
                            if (p.numBoids <= 0) // if negative set default
                                p.numBoids = defaultBoids; // default value
                        }

                    // BOID MASS
                    } else if (strncmp(line.c_str(), "mass: ", 6) == 0) {
                        readValue = sscanf(line.c_str(), "mass: %f", &p.boidMass);
                        if (readValue != 1) {
                            cout << "error reading in boids mass" << endl;
                            p.boidMass = 0.01;
                        }

                    // BIOD RANGE LEVELS
                    } else if (strncmp(line.c_str(), "ACG: ", 5) == 0) { // read in
                        readValue = sscanf(line.c_str(), "ACG: %f, %f, %f", &p.avoidanceRange, &p.cohesionRange, &p.maxSearchRange);
                        if (readValue != 3) {
                            cout << "error reading in boid ACG values" << endl;
                            p.avoidanceRange = 0.2;
                            p.cohesionRange = 0.4;
                            p.maxSearchRange = 0.6;
                        }

                    // BOID FORCE MULTIPLIERS
                    } else if (strncmp(line.c_str(), "ACG forces: ", 12) == 0) {
                        readValue = sscanf(line.c_str(), "ACG forces: %f, %f, %f", &p.avoidanceMultiplier, &p.cohesionMultiplier, &p.gatherMultiplier);
                        if (readValue != 3) {
                            cout << "error reading in boid ACG forces" << endl;
                            p.avoidanceMultiplier = 3.0;
                            p.cohesionMultiplier = 2.0;
                            p.gatherMultiplier = 2.5;
                        }

                    // ARENA RANGE
                    } else if (strncmp(line.c_str(), "arena: ", 7) == 0) {
                        readValue = sscanf(line.c_str(), "arena: %f", &p.arenaRadius);
                        if (readValue != 1) {
                            cout << "error reading in arena radius" << endl;
                            p.arenaRadius = 500.0;
                        }

                    // FORCE MULTIPLIER
                    } else if (strncmp(line.c_str(), "force: ", 7) == 0) {
                        readValue = sscanf(line.c_str(), "force: %f", &p.forceMultiplier);
                        if (readValue != 1) {
                            cout << "error reading in force multiplier" << endl;
                            p.forceMultiplier = 1.0;
                        }

                    // MIN VELOCITY
                    } else if (strncmp(line.c_str(), "min-velocity: ", 14) == 0) { // read in
                        readValue = sscanf(line.c_str(), "min-velocity: %f", &p.minVelocity);
                        if (readValue != 1) {
                            cout << "error reading in boid min-velocity" << endl;
                            p.minVelocity = 1.0f;
                        }

                    // MAX VELOCITY
                    } else if (strncmp(line.c_str(), "max-velocity: ", 14) == 0) { // read in
                        readValue = sscanf(line.c_str(), "max-velocity: %f", &p.maxVelocity);
                        if (readValue != 1) {
                            cout << "error reading in boid min-velocity" << endl;
                            p.maxVelocity = 15.0f;
                        }

                    // GRAPH INFORMATION
                    } else if (strncmp(line.c_str(), "total-buckets: ", 15) == 0) { // read in graph data
                        // get number of buckets that should be read in
                        readValue = sscanf(line.c_str(), "total-buckets: %u", &dataPoints);
                        if (readValue != 1) {
                            cout << "error reading in total buckets" << endl;
                            p.graphValues->clear(); // set to empty for default values
                        } else {
                            // read in all data points

                            if (dataPoints <= 0) {
                                // use set default values
                                p.graphValues->clear();
                            } else {
                                // get the values

                                for (unsigned int i = 0; i < (unsigned int)dataPoints; i++) {
                                    // read in each line
                                    getline(iFile, line);

                                    if (line.size() == 0) {
                                        // done reading if premature, disregard data and use defaults
                                        p.graphValues->clear(); // clear all elements so that defaults are used
                                    } else {
                                        // set each bucket value
                                        readValue = sscanf(line.c_str(), "%f", &dataValue);
                                        if (readValue != 1) {
                                            // error reading value in, use default values
                                            p.graphValues->clear(); // clear all elements so that defaults are used
                                            break;
                                        } else {
                                            // set value for data bucket at index i
                                            p.graphValues->push_back(dataValue);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // close the file
            try {
                iFile.close(); // close file when done
            } catch (const exception &e) {
                cout << filename << " failed to close" << endl;
                cout << e.what() << endl;
                return false;
            }
        } else {
            cout << "File failed to open" << endl;
            return false;
        }
    } catch(const exception &e) {
        cout << filename << " failed to open" << endl;
        cout << e.what() << endl;
        return false;
    }

    return true;
}

/**
 * To take the state information of the program and write it out to the
 * filename passed as a parameter.
 */
bool givr::fileIO::saveConfigFile(struct ProgramParameters &p,
                                  const string &filename) { // will need graph information and boid start state

    // write information to file
    try {
        ofstream oFile("../../" + filename);

        if (oFile.is_open()) {

            // write to file
            oFile << "#CONFIG FILE\n\n";


            // NUMBER OF BOIDS
            oFile << "# total number of boids to create\n";
            oFile << "boids: " << p.numBoids << "\n\n";


            // BOID MASS
            oFile << "# boids mass\n";
            oFile << "mass: " << p.boidMass << "\n\n";


            // BIOD RANGE LEVELS
            oFile << "# avoidance/cohesion, cohesion/gather, and max ranges\n";
            oFile << "ACG: " << p.avoidanceRange << ", "
                             << p.cohesionRange  << ", "
                             << p.maxSearchRange << "\n\n";

            // BOID FORCES
            oFile << "# boid force multipliers\n";
            oFile << "ACG forces: " << p.avoidanceMultiplier << ", "
                                    << p.cohesionMultiplier << ", "
                                    << p.gatherMultiplier << "\n\n";

            // ARENA RANGE
            oFile << "# radius of arena\n";
            oFile << "arena: " << p.arenaRadius << "\n\n";


            // FORCE MULTIPLIER
            oFile << "# force multiplier\n";
            oFile << "force: " << p.forceMultiplier << "\n\n";


            // MIN VELOCITY
            oFile << "# minimum velocity of boids\n";
            oFile << "min-velocity: " << p.minVelocity << "\n\n";


            // MAX VELOCITY
            oFile << "# maximum velocity of boids\n";
            oFile << "max-velocity: " << p.maxVelocity << "\n\n";


            // GRAPH INFORMATION
            oFile << "# get graph values\n";
            oFile << "total-buckets: " << p.graphValues->size() << "\n"; // total buckets to save
            for (unsigned int i = 0; i < p.graphValues->size(); i++) // total buckets
                oFile << p.graphValues->at(i) << "\n";
            oFile << "\n";
            oFile << "# eof";


            // close the file
            try {
                oFile.close(); // close file when done
            } catch (const exception &e) {
                cout << filename << " failed to close" << endl;
                cout << e.what() << endl;
                return false;
            }
        } else {
            cout << "File failed to open" << endl;
            return false;
        }
    } catch (const exception &e) {
        cout << filename << " failed to open" << endl;
        cout << e.what() << endl;
        return false;
    }

    return true;
}
