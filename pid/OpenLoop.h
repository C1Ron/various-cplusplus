#ifndef OPEN_LOOP_H
#define OPEN_LOOP_H
#include "Controller.h"

class OpenLoop : public Controller
{
public:
    OpenLoop(double dt);
    ~OpenLoop() = default;
    double update(double input) override;
    void setInput(double input) override;
    void setTimeStep(double dt) override;
    void setOutputLimits(double min, double max) override;
    double getTimeStep() const override;
    void reset() override;
private:
    double input;
    double dt;
    double outMin;
    double outMax;
    double clamp(double value, double min, double max);
};

#endif // OPEN_LOOP_H