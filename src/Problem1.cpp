#include "ControlSystem.hpp"
#include "Sensor.hpp"
#include "SensorFactory.hpp"

int main() {
    cout << "################ Question 1 ###################" << endl;
    
    AerospaceControlSystem ctrlSys;

    // Adding different types of sensors using the factory
    ctrlSys.addSensor(SensorFactory::createSensor("Altimeter"));
    ctrlSys.addSensor(SensorFactory::createSensor("PitotTube"));
    ctrlSys.addSensor(SensorFactory::createSensor("Gyroscope"));

    // Run monitoring and adjusting cycle
    ctrlSys.monitorAndAdjust();

    cout << "################ End of Question 1 ###################" << endl;
    
    return 0;
}