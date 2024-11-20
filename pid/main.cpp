#include <iostream>
#include "Pid.h"
#include "Simulator.h"
#include "FirstOrderSystem.h"
#include "SecondOrderSystem.h"
#include "MassSpringDamper.h"
#include "SetpointGenerator.h"

int main(int argc, char** argv) 
{
    double dtc = static_cast<double>(argc > 1) ? std::stod(argv[1]) : 0.5;
    double Kp = static_cast<double>(argc > 2) ? std::stod(argv[2]) : 2.0;
    double Ki = static_cast<double>(argc > 3) ? std::stod(argv[3]) : 5.0;
    double Kd = static_cast<double>(argc > 4) ? std::stod(argv[4]) : 0.0;
    double dt = 0.0001;       // Simulation time step
    double Tf = 50.0;        // Final time

    // System parameters
    double m = 1.0, c = 2.0, k = 5.0;

    // Create and configure PID controller
    PIDController pid(Kp, Ki, Kd, dtc);
    pid.setOutputLimits(-10.0, 10.0);
    pid.setIntegralLimit(10.0);

    MassSpringDamper system(m, c, k, 0.0, 0.0, 0.0);

    // Create setpoint generator
    // auto input = SetpointGenerator::createStep(1.0, 2.0, 0.0);
    auto input = SetpointGenerator::createRamp(0.3, 2.0, 1.0);

    // Create and run simulator
    Simulator simulator(system, pid, input, dt, Tf);
    simulator.run();
    simulator.plot();

    std::cout << "Simulation with separate timesteps complete. Press Enter to exit..." << std::endl;
    std::cin.get();
    simulator.close();


    return 0;
}
