#include <iostream>
#include "Pid.h"
#include "Simulator.h"
#include "FirstOrderSystem.h"
#include "SecondOrderSystem.h"
#include "SetpointGenerator.h"

int main() 
{
    // Create and configure PID controller
    PIDController pid(1.0, 0.1, 0); // Kp, Ki, Kd
    pid.setOutputLimits(-10.0, 10.0);
    pid.setIntegralLimit(10.0);
    pid.setFilterCoefficient(0.1);
    
    // SecondOrderSystem system(0.5, 1.0, 0.0, 0.0);  // ζ, ω, x0, v0;
    FirstOrderSystem system(1.0, 0.0);  // τ, x0
    
    // Create setpoint generator
    auto stepGen = SetpointGenerator::createRamp(1.0, 2.0, 1.0);
    
    // Create and run simulator
    Simulator simulator(pid, system, stepGen, 0.001, 100.0);
    
    // Run simulation and plot results
    simulator.run();
    simulator.plot();
    
    std::cout << "Simulation complete. Press Enter to exit..." << std::endl;
    std::cin.get();
    simulator.close();
    
    return 0;
}