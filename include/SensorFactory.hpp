#include "Sensor.hpp"

// SensorFactory class
class SensorFactory {
public:
    static unique_ptr<Sensor> createSensor(const string& sensorType) {
        // 
        if (sensorType == "Altimeter") {
            return make_unique<Altimeter>();
        } else if (sensorType == "PitotTube") {
            return make_unique<PitotTube>();
        } else if (sensorType == "AngleOfAttackProbe") {
            return make_unique<Gyroscope>();
        } else {
            return nullptr;
        }
    }
};