#ifndef CONTROLSYSTEM_HPP
#define CONTROLSYSTEM_HPP

#include "Sensor.hpp"
#include <vector>
using namespace std;

// AerospaceControlSystem class
class AerospaceControlSystem {
private:
    vector<unique_ptr<Sensor>> sensors;
public:
    // Function to create the sensor
    void addSensor(unique_ptr<Sensor> sensor) {
        sensors.push_back(std::move(sensor));
    }
    
    // Function to indicate we are adjusting controls based on sensor data
    void monitorAndAdjust() {
        for (auto& sensor : sensors) {
            // Calling function to gather sensor data
            sensor->gatherData();
            // Calling function to process sensor data
            sensor->processData();
            // Output message for adjusting controls
            cout << "Adjusting controls based on sensor data." << endl;
        }
    }

    ~AerospaceControlSystem() {
        for (auto& sensor : sensors) {
            // Output message that the sensor has been deleted
            cout << "Sensor deleted (" << sensor.get() << ") of type \"" << sensor->getType() << "\"" << endl;
        }
    }
};

#endif