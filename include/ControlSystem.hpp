#include "Sensor.hpp"
#include<vector>

// AerospaceControlSystem class
class AerospaceControlSystem {
private:
    // Setting up sensor pointer via a vector
    vector<unique_ptr<Sensor>> sensors;
public:
    // Function to add sensor using its pointer
    void addSensor(unique_ptr<Sensor> sensor) {
        sensors.push_back(move(sensor));
    }
    
    // Function to gather and process sensor data, then make adjustments based on the data
    void monitorAndAdjust() {
        for (auto& sensor : sensors) {
            // Set up for data collection
            sensor->gatherData();
            // Set up for data processing
            sensor->processData();
            // Output statement showing the system is adjusting based on sensor data
            cout << "Adjusting controls based on sensor data." << endl;
        }
    }
};