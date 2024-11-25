#include "Sensor.hpp"

// Sensor factory class
class SensorFactory {
public:
    static unique_ptr<Sensor> createSensor(const string& sensorType) {
        // Setting pointer for altimeter sensor string
        if (sensorType == "Altimeter") {
            return make_unique<Altimeter>();
        // Setting pointer for pitot tube sensor string
        } else if (sensorType == "PitotTube") {
            return make_unique<PitotTube>();
        // Setting pointer for gyroscope sensor string
        } else if (sensorType == "Gyroscope") {
            return make_unique<Gyroscope>();
        // If not sensor string is provided return a null pointer
        } else {
            return nullptr;
        }
    }
};