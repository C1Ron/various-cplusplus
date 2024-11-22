#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
    virtual ~Controller() = default;

    virtual double update(double input) = 0;
    virtual void setInput(double input) = 0;

    virtual void setTimeStep(double dt) = 0;
    virtual double getTimeStep() const = 0;

    virtual void setOutputLimits(double min, double max) = 0;

    virtual void reset() = 0;
};

#endif // CONTROLLER_H