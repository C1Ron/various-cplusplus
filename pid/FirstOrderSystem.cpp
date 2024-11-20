#include "FirstOrderSystem.h"

FirstOrderSystem::FirstOrderSystem(double timeConstant, double initPos)
    : timeConstant(timeConstant), position(initPos)
{
    // Intentionally empty
}

double FirstOrderSystem::diff(double u, double x) const
{
    // First order system equation
    // τ x' + x = u
    return (u - x) / timeConstant;
}

void FirstOrderSystem::integrate(double u, double dt)
{
    // RK4 integration
    double k1, k2, k3, k4;
    
    k1 = diff(u, position);
    
    k2 = diff(u, position + k1 * dt/2);
    
    k3 = diff(u, position + k2 * dt/2);
    
    k4 = diff(u, position + k3 * dt);
    
    position += (dt/6) * (k1 + 2*k2 + 2*k3 + k4);
}

double FirstOrderSystem::getX() const 
{ 
    return position; 
}

double FirstOrderSystem::getDX() const 
{ 
    return diff(0.0, position); 
}

void FirstOrderSystem::reset() 
{
    position = 0.0;
}