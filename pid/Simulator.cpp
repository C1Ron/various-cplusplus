#include "Simulator.h"
#include <iostream>
#include <cmath>

Simulator::Simulator(PIDController& pidController, System& sys,
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
        double control = pid.compute(measurement);
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
    fprintf(gnuplotPipe, "set terminal wxt 0 size 1600,600 enhanced font 'Arial,20' title 'System Response'\n");
    fprintf(gnuplotPipe, "set grid lw 2\n");
    fprintf(gnuplotPipe, "set key font 'Arial,20'\n");
    fprintf(gnuplotPipe, "set key spacing 2\n");
    fprintf(gnuplotPipe, "set key box lw 3\n");
    
    fprintf(gnuplotPipe, "set title 'System Response' font 'Arial,24'\n");
    fprintf(gnuplotPipe, "set xlabel 'Time (s)' font 'Arial,20'\n");
    fprintf(gnuplotPipe, "set ylabel 'Value' font 'Arial,20'\n");
    
    fprintf(gnuplotPipe, "set style line 1 lw 5 lc rgb 'red'\n");
    fprintf(gnuplotPipe, "set style line 2 lw 5 lc rgb 'blue'\n");
    
    fprintf(gnuplotPipe, "plot 'data.txt' using 1:2 title 'Setpoint' ls 1 with lines, ");
    fprintf(gnuplotPipe, "'data.txt' using 1:3 title 'Output' ls 2 with lines\n");
    
    // Second plot: Error and Control
    fprintf(gnuplotPipe, "set terminal wxt 1 size 1600,600 enhanced font 'Arial,20' title 'Error and Control'\n");
    fprintf(gnuplotPipe, "set grid lw 2\n");
    fprintf(gnuplotPipe, "set key font 'Arial,20'\n");
    fprintf(gnuplotPipe, "set key spacing 2\n");
    fprintf(gnuplotPipe, "set key box lw 3\n");
    
    fprintf(gnuplotPipe, "set title 'Error and Control Signal' font 'Arial,24'\n");
    fprintf(gnuplotPipe, "set xlabel 'Time (s)' font 'Arial,20'\n");
    fprintf(gnuplotPipe, "set ylabel 'Value' font 'Arial,20'\n");
    
    fprintf(gnuplotPipe, "set style line 1 lw 5 lc rgb 'purple'\n");
    fprintf(gnuplotPipe, "set style line 2 lw 5 lc rgb 'green'\n");
    
    fprintf(gnuplotPipe, "plot 'data.txt' using 1:($2-$3) title 'Error' ls 1 with lines, ");
    fprintf(gnuplotPipe, "'data.txt' using 1:4 title 'Control Signal' ls 2 with lines\n");
    
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

