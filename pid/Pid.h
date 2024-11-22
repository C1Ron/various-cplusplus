#ifndef PID_H
#define PID_H
#include "Controller.h"
#include <iostream>
#include <cmath>

class PIDController : public Controller
{
public:
    // Constructor and destructor
    PIDController(double Kp = 1.0, double Ki = 1.0, double Kd = 1.0, double dt = 0.01);
    ~PIDController() = default;

    void setInput(double input);
    void setTimeStep(double dt) override;
    double getTimeStep() const override;

    // Configuration methods
    void setGains(double Kp, double Ki, double Kd);
    void setOutputLimits(double min, double max) override;
    void setIntegralLimit(double limit);

    // Getters
    double getProportionalTerm() const;
    double getIntegralTerm() const;
    double getDerivativeTerm() const;
    double getError() const;
    void printGains() const;

    // Operation methods
    void reset() override;
    double update(double feedback) override;


private:
    // PID gains
    double Kp;  
    double Ki;  
    double Kd;  

    // Timing
    double dt;             
    
    // Control parameters
    double input;        
    double outputMin;       
    double outputMax;       
    double integralLimit;   
    
    // State variables
    double error;   
    double prevError;    
    double integral;        
    double feedback; 

    // Initialization flag
    bool firstRun;

    // Controller terms
    double calcPTerm(double error);
    double calcITerm(double error);
    double calcDTerm(double error);
    
    // Utility functions
    double clamp(double value, double min, double max);

};

#endif // PID_H