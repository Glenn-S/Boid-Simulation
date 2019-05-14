/**
 * Filename: boid.h
 * Author: Glenn Skelton
 *
 * Last Modified: March 27, 2019
 */

#include "givr.h"
#include <glm/gtc/matrix_transform.hpp>
#include "boid.h"

using namespace std;
using namespace givr;
using namespace givr::geometry;


// class: Boid

///////////////////////////////////// CONSTRUCTOR /////////////////////////////////
Boid::Boid(unsigned int a_ID,
           float a_mass,
           vec3f a_p,
           vec3f a_v,
           vec3f a_F) : m_ID(a_ID),
                        m_mass(a_mass),
                        m_p_init(a_p),
                        m_p(a_p),
                        m_v(a_v),
                        m_F(a_F) {}

Boid::~Boid() {}


///////////////////////////////// GETTERS/SETTERS ////////////////////////////////
signed int Boid::getID() const { return this->m_ID; }

float Boid::getMass() const { return this->m_mass; }
void Boid::setMass(const float &a_mass) { this->m_mass = a_mass; }

vec3f Boid::getInitialPosition() const { return this->m_p_init; }

vec3f Boid::getPosition() const { return this->m_p; }
void Boid::setPosition(const vec3f &a_p) { this->m_p = a_p; }

vec3f Boid::getLastForce() const { return this->m_lastForce; }
void Boid::setLastForce(const vec3f &a_lastForce) { this->m_lastForce = a_lastForce; }

vec3f Boid::getVelocity() const { return this->m_v; }
void Boid::setVelocity(const vec3f &a_v) { this->m_v = a_v; }

vec3f Boid::getNetForce() const { return this->m_F; }
void Boid::setNetForce(const vec3f &a_F) { this->m_F = a_F; }
void Boid::addNetForce(const vec3f &a_F) { this->m_F += a_F; }

mat4f Boid::getModelMat() const { return this->m_model; }
void Boid::setModelMat(const mat4f &a_model) { this->m_model = a_model; }


////////////////////////////////// FUNCTIONS /////////////////////////////////////


/**
 * To calculate the force of the boundary to apply to the boid to
 * keep them contained.
 */
void Boid::calculateBoundaryForce(const float &a_arena, const float &a_forceMultiply) {
    vec3f force(0, 0, 0);
    vec3f normal = this->getPosition() - vec3f(0, 0, 0); // from center of arena
    float dist = glm::length(normal);
    normal = glm::normalize(normal); // techincally normal vector

    if (dist >= a_arena) { // push boid back in from arena bounds
        force += -normal; // apply normal force
        force += this->getVelocity() - (glm::dot(this->getVelocity(), normal) * normal); // apply tangential force
        this->addNetForce((dist - a_arena) * force * a_forceMultiply);
    }
}

/**
 * To update the position of the boid using semi-implicit integration.
 * If the value obtained for the velocity is to high or low, the max/min
 * values passed in will be used instead respectively.
 */
void Boid::updateBoidPosition(const float &a_t,
                              const float &a_v_min,
                              const float &a_v_max) {

    vec3f acceleration = this->getNetForce() / this->getMass(); // a = F / m
    vec3f velocity = this->getVelocity() + (acceleration * a_t);

    // check and set velocity
    if (glm::length(velocity) < a_v_min)
        velocity = glm::normalize(velocity) * a_v_min;
    else if (glm::length(velocity) > a_v_max)
        velocity = glm::normalize(velocity) * a_v_max;

    this->setVelocity(velocity); // V = V + a(delta_t)
    this->setPosition(this->getPosition() + (this->getVelocity() * a_t)); // X = x + v(delta_t)
    this->setLastForce(this->getNetForce()); // keep track of when force gets cleared
    this->setNetForce(vec3f(0.0f, 0.0f, 0.0f)); // reset force accumulator
}
