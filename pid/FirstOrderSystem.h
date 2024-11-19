#ifndef FIRST_ORDER_SYSTEM_H
#define FIRST_ORDER_SYSTEM_H
#include "System.h"
#include <iostream>
#include <cmath>

class FirstOrderSystem : public System
{
public:
    FirstOrderSystem(double timeConstant, double initPos);
    
    void update(double input, double dt) override;
    double getPosition() const override;
    double getVelocity() const override;
    void reset() override;

private:
    double timeConstant;
    double position;
    double velocity(double u, double x) const;
};
#endif // FIRST_ORDER_SYSTEM_H