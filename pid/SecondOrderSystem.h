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
    double acceleration(double u, double x, double v) const;
    
public:
    SecondOrderSystem(double damping, double natFreq,
                    double initPos, double initVel);
    
    void update(double input, double dt) override;
    double getPosition() const override;
    double getVelocity() const override;
};
#endif // SECOND_ORDER_SYSTEM_H