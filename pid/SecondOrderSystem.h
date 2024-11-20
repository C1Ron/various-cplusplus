#ifndef SECOND_ORDER_SYSTEM_H
#define SECOND_ORDER_SYSTEM_H
#include "System.h"
#include <iostream>
#include <cmath>

class SecondOrderSystem : public System
{
private:
    double natFreq;
    double damping;
    double position;
    double velocity;
    double diff(double u, double x, double v) const;
    
public:
    SecondOrderSystem(double damping, double natFreq,
                    double initPos, double initVel);
    
    void integrate(double input, double dt) override;
    double getX() const override;
    double getDX() const override;
    void reset() override;
};
#endif // SECOND_ORDER_SYSTEM_H