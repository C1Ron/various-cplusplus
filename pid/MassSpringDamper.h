#ifndef MASS_SPRING_DAMPER_H
#define MASS_SPRING_DAMPER_H
#include "System.h"
#include <iostream>
#include <cmath>

class MassSpringDamper : public System
{   
public:
    MassSpringDamper(double m, double c, double k, double L0, double x0, double Dx0);
    
    void integrate(double input, double dt) override;
    double getX() const override;
    double getDX() const override;
    void reset() override;
private:
    double m;   // mass
    double c;   // damping coefficient
    double k;   // spring constant
    double L0;  // no-load spring length
    double x;
    double Dx;
    double diff(double u, double x, double Dx) const;
};
#endif // MASS_SPRING_DAMPER_H