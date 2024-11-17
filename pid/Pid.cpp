#include "Pid.h"
#include <cmath>

double PIDController::clamp(double value, double min, double max) 
{
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

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
    integral(0.0),
    measurement(0.0),
    filterCoeff(0.1),
    errorFiltered(0.0),
    firstRun(true)
{}

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

void PIDController::setFilterCoefficient(double coeff) 
{
    filterCoeff = clamp(coeff, 0.0, 1.0);
}

void PIDController::setSampleTime(double newDt) 
{
    if (newDt > 0) {
        Ki *= dt / newDt;
        Kd *= newDt / dt;
        dt = newDt;
    }
}

void PIDController::reset() 
{
    integral = 0.0;
    error = 0.0;
    errorFiltered = 0.0;
    measurement = 0.0;
    firstRun = true;
}

double PIDController::compute(double measurementNew) 
{
    // Calculate error
    double errorNew = setpoint - measurementNew;
    
    // Initialize on first run
    if (firstRun) {
        error = errorNew;
        errorFiltered = errorNew;
        measurement = measurementNew;
        firstRun = false;
        return 0.0;
    }
    
    // Proportional term
    double pTerm = Kp * errorNew;
    
    // Integral term with anti-windup
    integral += Ki * errorNew * dt;
    integral = clamp(integral, -integralLimit, integralLimit);
    double iTerm = integral;
    
    // Derivative term with filtering
    double errorDifference = errorNew - error;
    errorFiltered = filterCoeff * errorDifference + 
                       (1.0 - filterCoeff) * errorFiltered;
    
    // Use filtered derivative
    double dTerm = Kd * (errorFiltered / dt);
    
    // Calculate total output
    double output = pTerm + iTerm + dTerm;
    
    // Clamp output to limits
    output = clamp(output, outputMin, outputMax);
    
    // Store error for next iteration
    error = errorNew;
    measurement = measurementNew;
    
    return output;
}

double PIDController::getProportionalTerm() const 
{
    return Kp * (setpoint - measurement);
}

double PIDController::getIntegralTerm() const 
{
    return integral;
}

double PIDController::getDerivativeTerm() const 
{
    return Kd * errorFiltered / dt;
}

double PIDController::getError() const 
{
    return error;
}