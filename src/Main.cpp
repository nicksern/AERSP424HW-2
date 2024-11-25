#include "ControlSystem.hpp"
#include "Sensor.hpp"
#include "SensorFactory.hpp"

int main() {
    AerospaceControlSystem ctrlSys;

    // Adding different types of sensors using the factory
    ctrlSys.addSensor(SensorFactory::createSensor("Altimeter"));
    ctrlSys.addSensor(SensorFactory::createSensor("PitotTube"));
    ctrlSys.addSensor(SensorFactory::createSensor("AngleOfAttackProbe"));

    // Run monitoring and adjusting cycle
    ctrlSys.monitorAndAdjust();

    return 0;
}