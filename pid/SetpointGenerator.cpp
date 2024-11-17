// SetpointGenerator.cpp
#include "SetpointGenerator.h"
#include <cmath>
#include <random>

// Private constructor
SetpointGenerator::SetpointGenerator(SignalType type, double amplitude, double frequency,
                                   double offset, double slope, double timeDelay)
    : type(type)
    , amplitude(amplitude)
    , frequency(frequency)
    , offset(offset)
    , slope(slope)
    , lastValue(offset)
    , timeDelay(timeDelay)
{}

// Factory method for step input
SetpointGenerator SetpointGenerator::createStep(double stepAmplitude, double stepDelay, 
                                              double initialValue) {
    return SetpointGenerator(SignalType::STEP, stepAmplitude, 0.0, initialValue, 
                           0.0, stepDelay);
}

// Factory method for ramp input
SetpointGenerator SetpointGenerator::createRamp(double rampSlope, double rampDelay,
                                              double finalAmplitude, double initialValue) {
    return SetpointGenerator(SignalType::RAMP, finalAmplitude, 0.0, initialValue,
                           rampSlope, rampDelay);
}

// Factory method for sine wave
SetpointGenerator SetpointGenerator::createSine(double sineAmplitude, double sineFrequency,
                                              double sineDelay, double sineOffset) {
    return SetpointGenerator(SignalType::SINE, sineAmplitude, sineFrequency, 
                           sineOffset, 0.0, sineDelay);
}

// Factory method for square wave
SetpointGenerator SetpointGenerator::createSquare(double squareAmplitude, 
                                                double squareFrequency,
                                                double squareOffset) {
    return SetpointGenerator(SignalType::SQUARE, squareAmplitude, squareFrequency,
                           squareOffset, 0.0, 0.0);
}

// Factory method for random steps
SetpointGenerator SetpointGenerator::createRandomSteps(double stepAmplitude, 
                                                     double stepInterval,
                                                     double initialValue) {
    return SetpointGenerator(SignalType::RANDOM_STEPS, stepAmplitude, 1.0/stepInterval,
                           initialValue, 0.0, 0.0);
}

double SetpointGenerator::getValue(double time) {
    switch(type) {
        case SignalType::STEP: {
            if (time < timeDelay) {
                return offset;
            }
            return offset + amplitude;
        }
        
        case SignalType::RAMP: {
            if (time < timeDelay) {
                return offset;
            }
            double value = offset + slope * (time - timeDelay);
            if (slope > 0) {
                return std::min(value, offset + amplitude);
            } else {
                return std::max(value, offset + amplitude);
            }
        }
        
        case SignalType::SINE: {
            if (time < timeDelay) {
                return offset;
            }
            return offset + amplitude * 
                   std::sin(2.0 * M_PI * frequency * (time - timeDelay));
        }
        
        case SignalType::SQUARE: {
            return offset + amplitude * 
                   (std::sin(2.0 * M_PI * frequency * time) > 0 ? 1.0 : -1.0);
        }
        
        case SignalType::RANDOM_STEPS: {
            // Change value every 1/frequency seconds
            if (std::fmod(time * frequency, 1.0) < 0.001) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                static std::uniform_real_distribution<> dis(-1.0, 1.0);
                lastValue = offset + amplitude * dis(gen);
            }
            return lastValue;
        }
        
        default:
            return offset;
    }
}