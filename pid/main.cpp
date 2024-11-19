#include <iostream>
#include "Pid.h"
#include "Simulator.h"
#include "FirstOrderSystem.h"
#include "SecondOrderSystem.h"
#include "MassSpringDamper.h"
#include "SetpointGenerator.h"

int main() 
{
    // Simulation parameters
    double dt =  0.0001;     // Simulator time step
    double dtc = 0.01;       // Controller time step
    double Tf = 50.0;        // Final time

    double Kp = 1.0, Ki = 1.0, Kd = 0.0;
    double m = 1.0, c = 2.0, k = 5.0;

    // Create and configure PID controller
    PIDController pid(Kp, Ki, Kd, dtc);
    pid.setOutputLimits(-10.0, 10.0);
    pid.setIntegralLimit(10.0);

    MassSpringDamper system(m, c, k, 0.0, 0.0, 0.0);

    // Create setpoint generator
    auto stepGen = SetpointGenerator::createStep(1.0, 2.0, 0.0);

    // Create and run simulator
    Simulator simulator(system, pid, stepGen, dt, Tf);
    simulator.run();
    simulator.plot();

    std::cout << "Simulation with separate timesteps complete. Press Enter to exit..." << std::endl;
    std::cin.get();
    simulator.close();


    return 0;
}
