// Included to run
#include "HW2Problems.hpp"
// Everything else
#include "ControlSystem.hpp"
#include "Sensor.hpp"
#include "SensorFactory.hpp"
using namespace std;

// Problem 1 function to be run in Main
void runProblem1() {
    cout << "################ Question 1 ###################" << endl;
    { // Prevent deleting the sensors AFTER the end of question 1 banner
    AerospaceControlSystem ctrlSys;
    // Adding different types of sensors using the factory
    ctrlSys.addSensor(SensorFactory::createSensor("Altimeter"));
    ctrlSys.addSensor(SensorFactory::createSensor("PitotTube"));
    ctrlSys.addSensor(SensorFactory::createSensor("Gyroscope"));
    // Run monitoring and adjusting cycle
    ctrlSys.monitorAndAdjust();
    }
    cout << "################ End of Question 1 ###################" << endl;
}