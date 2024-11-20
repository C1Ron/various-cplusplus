#include "MassSpringDamper.h"

MassSpringDamper::MassSpringDamper(double m, double c, double k, double L0, double x0, double Dx0)
    : m(m), c(c), k(k), L0(L0), x(x0), Dx(Dx0)
{
    // Intentionally empty
}

double MassSpringDamper::diff(double u, double x, double Dx) const 
{
    // Second order system equation
    // DDx = (F - c*Dx - k*x) / m
    return (u - c*Dx - k*(x - L0)) / m;
}

void MassSpringDamper::integrate(double u, double dt) 
{
    // RK4 integration
    double k1_v, k1_a, k2_v, k2_a, k3_v, k3_a, k4_v, k4_a;
    
    k1_v = Dx;
    k1_a = diff(u, x, Dx);
    
    k2_v = Dx + k1_a * dt/2;
    k2_a = diff(u, x + k1_v * dt/2, k1_v + k1_a * dt/2);
    
    k3_v = Dx + k2_a * dt/2;
    k3_a = diff(u, x + k2_v * dt/2, k2_v + k2_a * dt/2);
    
    k4_v = Dx + k3_a * dt;
    k4_a = diff(u, x + k3_v * dt, k3_v + k3_a * dt);
    
    x += (dt/6) * (k1_v + 2*k2_v + 2*k3_v + k4_v);
    Dx += (dt/6) * (k1_a + 2*k2_a + 2*k3_a + k4_a);
}

double MassSpringDamper::getX() const
{ 
    return x; 
}
double MassSpringDamper::getDX() const 
{ 
    return Dx; 
}

void MassSpringDamper::reset() 
{
    x = 0.0;
    Dx = 0.0;
}