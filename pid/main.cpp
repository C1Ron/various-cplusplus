#include <iostream>
#include "Pid.h"
#include "Simulator.h"
#include "FirstOrderSystem.h"
#include "SecondOrderSystem.h"
#include "SetpointGenerator.h"

int main() 
{
    // Simulation parameters
    double dt = 0.001;  // Time step
    double Tf = 50.0;   // Final time

    double Kp = 10.0;
    double Ki = 0.5;
    double Kd = 10.0;
    // Create and configure PID controller
    PIDController pid(Kp, Ki, Kd);
    pid.setOutputLimits(-10.0, 10.0);
    pid.setIntegralLimit(10.0);
    
    SecondOrderSystem system(0.4, 1.5, 0.0, 0.0);  // ζ, ω, x0, v0;
    // FirstOrderSystem system(1.0, 0.0);  // τ, x0
    
    // Create setpoint generator
    auto stepGen = SetpointGenerator::createStep(1.0, 2.0, 0.0);
    
    // Create and run simulator
    Simulator simulator(system, pid, stepGen, dt, Tf);
    
    // Run simulation and plot results
    simulator.run();
    simulator.plot();
    
    std::cout << "Simulation complete. Press Enter to exit..." << std::endl;
    std::cin.get();
    simulator.close();
    
    return 0;
}