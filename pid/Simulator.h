#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <fstream>
#include <vector>
#include <string>
#include "Pid.h"
#include "SetpointGenerator.h"
#include "System.h"

class Simulator 
{
public:
    Simulator(System& system, PIDController& pid, SetpointGenerator& generator,
             double dt = 0.001, double simTime = 10.0);
    ~Simulator();
    
    void run();
    void plot();
    void close();
    void reset(double newDt, double newSimTime);
    
private:

    PIDController& pid;
    System& system;
    SetpointGenerator& setpointGen;
    
    double dt;      // Simulation time step
    double simTime; // Total simulation time

    std::ofstream dataFile;
    FILE* gnuplotPipe;
    
    void saveDataPoint(double time, double setpoint, double output, double control);
    void cleanup();
};

#endif // SIMULATOR_H