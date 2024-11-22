#include "OpenLoop.h"

OpenLoop::OpenLoop(double dt) :
    input(0.0),
    dt(dt),
    outMin(-1.0),
    outMax(1.0)
{}

double OpenLoop::update(double feedback)
{
    return clamp(input, outMin, outMax);
}

void OpenLoop::setInput(double input)
{
    this->input = input;
}

void OpenLoop::setTimeStep(double dt)
{
    this->dt = dt;
}

double OpenLoop::getTimeStep() const
{
    return dt;
}

void OpenLoop::setOutputLimits(double min, double max)
{
    outMin = min;
    outMax = max;
}

void OpenLoop::reset()
{
    input = 0.0;
    dt = 0.01;
}

double OpenLoop::clamp(double value, double min, double max)
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}