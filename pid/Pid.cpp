#include "Pid.h"
#include <cmath>

PIDController::PIDController(double kp, double ki, double kd, double dt) :
    Kp(kp),
    Ki(ki),
    Kd(kd),
    dt(dt),
    setpoint(0.0),
    outputMin(-INFINITY),
    outputMax(INFINITY),
    integralLimit(INFINITY),
    error(0.0),
    prevError(0.0),
    integral(0.0),
    measurement(0.0),
    firstRun(true)
{}

double PIDController::calcPTerm(double error) 
{
    return Kp * error;
}

double PIDController::calcITerm(double error) 
{
    integral += Ki * error * dt;
    integral = clamp(integral, -integralLimit, integralLimit);
    return integral;
}

double PIDController::calcDTerm(double error) 
{
    double diff = (error - prevError) / dt;
    prevError = error;
    return Kd * diff;
}

void PIDController::setGains(double kp, double ki, double kd) 
{
    this->Kp = kp;
    this->Ki = ki;
    this->Kd = kd;
}

void PIDController::setSetpoint(double setpoint) 
{
    this->setpoint = setpoint;
}

void PIDController::setOutputLimits(double min, double max) 
{
    if (min < max) {
        outputMin = min;
        outputMax = max;
        integral = clamp(integral, outputMin, outputMax);
    }
}

void PIDController::setIntegralLimit(double limit) 
{
    integralLimit = fabs(limit);
}

void PIDController::setSampleTime(double newDt) 
{
    if (newDt > 0) {
        Ki *= dt / newDt;   // Scale integral term to maintain integral value
        Kd *= dt / newDt;   // Scale derivative term to maintain derivative value
        dt = newDt;
    }
}

double PIDController::getProportionalTerm() const 
{
    return Kp * error;
}

double PIDController::getIntegralTerm() const 
{
    return integral;
}

double PIDController::getDerivativeTerm() const 
{
    return Kd * error;
}

double PIDController::getError() const 
{
    return error;
}

void PIDController::reset() 
{
    integral = 0.0;
    error = 0.0;
    prevError = 0.0;
    measurement = 0.0;
    firstRun = true;
}

double PIDController::update(double m) 
{
    // Calculate error
    double e = setpoint - m;
    
    // Initialize on first run
    if (firstRun) {
        error = e;
        prevError = e;
        measurement = m;
        firstRun = false;
        return 0.0;
    }
    
    // Proportional term =======================================
    double pTerm = calcPTerm(e);
    double iTerm = calcITerm(e);
    double dTerm = calcDTerm(e);
    
    // Calculate total output
    double output = pTerm + iTerm + dTerm;
    
    // Clamp output to limits
    output = clamp(output, outputMin, outputMax);
    
    // Store error for next iteration
    error = e;
    measurement = m;
    
    return output;
}

double PIDController::clamp(double value, double min, double max) 
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

