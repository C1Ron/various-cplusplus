#include "Simulator.h"
#include <iostream>
#include <cmath>

Simulator::Simulator(System& sys, PIDController& pidController, 
                    SetpointGenerator& generator, double timestep, double time)
    : pid(pidController)
    , system(sys)
    , setpointGen(generator)
    , dt(timestep)
    , simTime(time)
{
    dataFile.open("data.txt");
    // Redirect stderr to /dev/null when opening gnuplot
    gnuplotPipe = popen("gnuplot 2>/dev/null", "w");
}

Simulator::~Simulator() 
{
    cleanup();
}

void Simulator::saveDataPoint(double time, double setpoint, 
                            double output, double control) {
    dataFile << time << " " << setpoint << " " 
            << output << " " << control << std::endl;
}

void Simulator::run() {
    const int steps = static_cast<int>(simTime / dt);
    std::cout << "Running simulation...\n";
    
    for (int i = 0; i < steps; i++) {
        double time = i * dt;
        
        // Get setpoint
        double setpoint = setpointGen.getValue(time);
        pid.setSetpoint(setpoint);
        
        // Compute control and update system
        double measurement = system.getPosition();
        double control = pid.update(measurement);
        system.update(control, dt);
        
        // Save data (every 10ms)
        if (i % 10 == 0) {
            saveDataPoint(time, setpoint, measurement, control);
        }
    }
}

void Simulator::plot() {
    if (!gnuplotPipe) {
        std::cerr << "Error: gnuplot pipe is not open" << std::endl;
        return;
    }

    // First plot: System Response
    fprintf(gnuplotPipe, "set terminal qt 0 size 1600,600 font 'Arial,16'\n");
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title 'System Response'\n");
    fprintf(gnuplotPipe, "set xlabel 'Time (s)'\n");
    fprintf(gnuplotPipe, "set ylabel 'Value'\n");
    fprintf(gnuplotPipe, "plot 'data.txt' using 1:2 title 'Setpoint' with lines lw 2 lc 'red', ");
    fprintf(gnuplotPipe, "'data.txt' using 1:3 title 'Output' with lines lw 2 lc 'blue'\n");

    // Second plot: Error and Control
    fprintf(gnuplotPipe, "set terminal qt 1 size 1600,600 font 'Arial,16'\n");
    fprintf(gnuplotPipe, "set grid\n");
    fprintf(gnuplotPipe, "set title 'Error and Control Signal'\n");
    fprintf(gnuplotPipe, "set xlabel 'Time (s)'\n");
    fprintf(gnuplotPipe, "set ylabel 'Value'\n");
    fprintf(gnuplotPipe, "plot 'data.txt' using 1:($2-$3) title 'Error' with lines lw 2 lc 'purple', ");
    fprintf(gnuplotPipe, "'data.txt' using 1:4 title 'Control Signal' with lines lw 2 lc 'green'\n");

    fflush(gnuplotPipe);

}

void Simulator::close() 
{
    cleanup();
}

void Simulator::cleanup() {
    if (gnuplotPipe) {
        // Close both windows explicitly
        fprintf(gnuplotPipe, "set terminal wxt 0\n");
        fprintf(gnuplotPipe, "unset output\n");
        fprintf(gnuplotPipe, "set terminal wxt 1\n");
        fprintf(gnuplotPipe, "unset output\n");
        fprintf(gnuplotPipe, "quit\n");
        fflush(gnuplotPipe);
        pclose(gnuplotPipe);
        gnuplotPipe = nullptr;
    }
    if (dataFile.is_open()) {
        dataFile.close();
    }
}

