// SetpointGenerator.h
#ifndef SETPOINT_GENERATOR_H
#define SETPOINT_GENERATOR_H

enum class SignalType {
    STEP,
    RAMP,
    SINE,
    SQUARE,
    RANDOM_STEPS
};

class SetpointGenerator 
{
    // Constructor is private - use factory methods to create instances
public:
    // Factory methods for each type
    static SetpointGenerator createStep(double stepAmplitude, double stepDelay, 
                                      double initialValue = 0.0);
    
    static SetpointGenerator createRamp(double rampSlope, double rampDelay, 
                                      double finalAmplitude, double initialValue = 0.0);
    
    static SetpointGenerator createSine(double sineAmplitude, double sineFrequency,
                                      double sineDelay, double sineOffset = 0.0);
    
    static SetpointGenerator createSquare(double squareAmplitude, double squareFrequency,
                                        double squareOffset = 0.0);
    
    static SetpointGenerator createRandomSteps(double stepAmplitude, double stepInterval,
                                             double initialValue = 0.0);
    
    // Get the setpoint value at a given time
    double getValue(double time);
    
private:
    SignalType type;
    double amplitude;   // For all types
    double frequency;   // For sine and square
    double offset;      // For all types
    double slope;       // For ramp
    double lastValue;   // For random steps
    double timeDelay;   // For ramp, step and sine
    
    // Private constructor - only used by factory methods
    SetpointGenerator(SignalType type, double amplitude, double frequency, 
                     double offset, double slope, double timeDelay);
};

#endif // SETPOINT_GENERATOR_H