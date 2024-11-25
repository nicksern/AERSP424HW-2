#include "Sensor.hpp"
#include<vector>

// AerospaceControlSystem class
class AerospaceControlSystem {
private:
    vector<unique_ptr<Sensor>> sensors;
public:
    void addSensor(unique_ptr<Sensor> sensor) {
        sensors.push_back(move(sensor));
    }
    
    void monitorAndAdjust() {
        for (auto& sensor : sensors) {
            sensor->gatherData();
            sensor->processData();
            cout << "Adjusting controls based on sensor data." << endl;
        }
    }
};