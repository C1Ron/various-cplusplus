#include "SecondOrderSystem.h"

SecondOrderSystem::SecondOrderSystem(double damping, double natFreq,
                                    double initPos, double initVel)
    : natFreq(natFreq), damping(damping), position(initPos), velocity(initVel)
{
    // Intentionally empty
}

double SecondOrderSystem::diff(double u, double x, double v) const 
{
    // Second order system equation
    // x'' + 2 ζ ω x' + ω^2 x = u
    return u - 2*damping*natFreq*v - natFreq*natFreq*x;
}

void SecondOrderSystem::integrate(double u, double dt) 
{
    // RK4 integration
    double k1_v, k1_a, k2_v, k2_a, k3_v, k3_a, k4_v, k4_a;
    
    k1_v = velocity;
    k1_a = diff(u, position, velocity);
    
    k2_v = velocity + k1_a * dt/2;
    k2_a = diff(u, position + k1_v * dt/2, k1_v + k1_a * dt/2);
    
    k3_v = velocity + k2_a * dt/2;
    k3_a = diff(u, position + k2_v * dt/2, k2_v + k2_a * dt/2);
    
    k4_v = velocity + k3_a * dt;
    k4_a = diff(u, position + k3_v * dt, k3_v + k3_a * dt);
    
    position += (dt/6) * (k1_v + 2*k2_v + 2*k3_v + k4_v);
    velocity += (dt/6) * (k1_a + 2*k2_a + 2*k3_a + k4_a);
}

double SecondOrderSystem::getX() const
{ 
    return position; 
}
double SecondOrderSystem::getDX() const 
{ 
    return velocity; 
}

void SecondOrderSystem::reset() 
{
    position = 0.0;
    velocity = 0.0;
}
