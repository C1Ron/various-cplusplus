#ifndef PID_H
#define PID_H

class PIDController 
{
public:
    // Constructor and destructor
    PIDController(double Kp = 1.0, double Ki = 1.0, double Kd = 1.0, double dt = 0.01);
    ~PIDController() = default;
    
    // Configuration methods
    void setGains(double Kp, double Ki, double Kd);
    void setSetpoint(double setpoint);
    void setOutputLimits(double min, double max);
    void setIntegralLimit(double limit);
    void setFilterCoefficient(double coeff);
    void setSampleTime(double dt);

    // Getters
    double getProportionalTerm() const;
    double getIntegralTerm() const;
    double getDerivativeTerm() const;
    double getError() const;

    // Operation methods
    void reset();
    double compute(double measurement);


private:
    // PID gains
    double Kp;  
    double Ki;  
    double Kd;  

    // Timing
    double dt;             
    
    // Control parameters
    double setpoint;        
    double outputMin;       
    double outputMax;       
    double integralLimit;   
    
    // State variables
    double error;       
    double integral;        
    double measurement; 
    double filterCoeff;     
    double errorFiltered;

    // Initialization flag
    bool firstRun;
    
    // Utility functions
    double clamp(double value, double min, double max);

};

#endif // PID_H