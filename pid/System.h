#ifndef SYSTEM_H
#define SYSTEM_H

class System
{
public:
    virtual ~System() = default;
    virtual double getPosition() const = 0;
    virtual double getVelocity() const = 0;
    virtual void update(double u, double dt) = 0;
    virtual void reset() = 0;
};

#endif // SYSTEM_H