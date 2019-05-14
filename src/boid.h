/**
 * Filename: boid.h
 * Author: Glenn Skelton
 *
 * Last Modified: March 27, 2019
 */

#ifndef BOID_H
#define BOID_H


#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace givr;
using namespace givr::geometry;


class Boid {
// public functions
public:
    ////////////////////////////////// CONSTRUCTORS /////////////////////////////////
    Boid(unsigned int a_ID,
         float a_mass,
         vec3f a_p,
         vec3f a_v,
         vec3f a_F);
    ~Boid();


    //////////////////////////////// GETTERS/SETTERS ////////////////////////////////
    signed int getID() const;

    float getMass() const;
    void setMass(const float &a_mass);

    vec3f getInitialPosition() const;

    vec3f getPosition() const;
    void setPosition(const vec3f &a_p);

    vec3f getVelocity() const;
    void setVelocity(const vec3f &a_v);

    vec3f getLastForce() const;
    void setLastForce(const vec3f &a_lastForce);

    vec3f getNetForce() const;
    void setNetForce(const vec3f &a_F);
    void addNetForce(const vec3f &a_F);

    mat4f getModelMat() const;
    void setModelMat(const mat4f &a_model);

    /////////////////////////////// HELPER FUNCTIONS ///////////////////////////////////
    void calculateBoundaryForce(const float &a_arena, const float &a_forceMultiply);

    void updateBoidPosition(const float &a_t,
                            const float &a_v_min,
                            const float &a_v_max);

// private variables
private:
    signed int m_ID;
    float m_mass;
    vec3f m_p_init;
    vec3f m_p;
    vec3f m_v;
    vec3f m_F; // force accumultor
    vec3f m_lastForce;
    mat4f m_model; // model matrix for transformations

    vec3f m_normal;


}; // class Boid



// OPERATOR OVERLOADS
inline std::string to_string(const Boid &b) {
    return string("ID: " + to_string(b.getID())) + "\n" +
           string("Position: " + to_string(b.getPosition())) + "\n" +
           string("Velocity: " + to_string(b.getVelocity())) + "\n" +
           string("Net Force: " + to_string(b.getNetForce())) + "\n";
}


inline std::ostream& operator<<(std::ostream &out, const Boid &b) {
    return out << "ID: " << b.getID() << endl <<
           "Position: " << b.getPosition() << endl <<
           "Velocity: " << b.getVelocity() << endl <<
           "Net Force: " << b.getNetForce() << endl;
}

inline bool operator==(const Boid &lhs, const Boid &rhs) {
    return lhs.getID() == rhs.getID();
}

inline bool operator!=(const Boid &lhs, const Boid &rhs) {
    return !(lhs == rhs);
}

#endif // BOID_H
