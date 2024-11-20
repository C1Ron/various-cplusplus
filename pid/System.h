#ifndef SYSTEM_H
#define SYSTEM_H

class System
{
public:
    virtual ~System() = default;
    virtual double getX() const = 0;
    virtual double getDX() const = 0;
    virtual void integrate(double u, double dt) = 0;
    virtual void reset() = 0;
};

#endif // SYSTEM_H