#include "FirstOrderSystem.h"

FirstOrderSystem::FirstOrderSystem(double timeConstant, double initPos)
    : timeConstant(timeConstant), position(initPos)
{
    // Intentionally empty
}

double FirstOrderSystem::velocity(double u, double x) const
{
    // First order system equation
    // τ x' + x = u
    return (u - x) / timeConstant;
}

void FirstOrderSystem::update(double u, double dt)
{
    // RK4 integration
    double k1, k2, k3, k4;
    
    k1 = velocity(u, position);
    
    k2 = velocity(u, position + k1 * dt/2);
    
    k3 = velocity(u, position + k2 * dt/2);
    
    k4 = velocity(u, position + k3 * dt);
    
    position += (dt/6) * (k1 + 2*k2 + 2*k3 + k4);
}

double FirstOrderSystem::getPosition() const 
{ 
    return position; 
}

double FirstOrderSystem::getVelocity() const 
{ 
    return velocity(0.0, position); 
}

void FirstOrderSystem::reset() 
{
    position = 0.0;
}