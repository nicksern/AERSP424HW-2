#ifndef SENSORFACTORY_HPP
#define SENSORFACTORY_HPP


#include "Sensor.hpp"

// Sensor factory class
class SensorFactory {
public:
    static unique_ptr<Sensor> createSensor(const string& sensorType) {
        // If sensor is an altimeter
        if (sensorType == "Altimeter") {
            // Calling to obtain altimeter sensor address
            auto sensor = make_unique<Altimeter>();
            // Output message of altimeter sensor creation and obtaining its name as a string
            cout << "Sensor created (" << sensor.get() << ") of type \"" << sensor->getType() << "\"" << endl;
            return sensor;
        // If sensor is a pitot tube
        } else if (sensorType == "PitotTube") {
            // Calling to obtain pitot tube address
            auto sensor = make_unique<PitotTube>();
            // Output message of pitot tube sensor creation and obtaining its name as a string
            cout << "Sensor created (" << sensor.get() << ") of type \"" << sensor->getType() << "\"" << endl;
            return sensor;
        // If sensor is a gyroscope
        } else if (sensorType == "Gyroscope") {
            // Calling to obtain gyroscope address
            auto sensor = make_unique<Gyroscope>();
            // Output message of gyroscope sensor creation and obtaining its name as a string
            cout << "Sensor created (" << sensor.get() << ") of type \"" << sensor->getType() << "\"" << endl;
            return sensor;
        // Else statement to return a null pointer if none of the three sensor types are indicated
        } else {
            return nullptr;
        }
    }
};

#endif