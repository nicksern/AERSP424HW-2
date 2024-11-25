#include<iostream>
#include<memory>
using namespace std;


// Abstract base class for sensors
class Sensor {

public:
    virtual ~Sensor() {}
    // Virtual data collection
    virtual void gatherData() = 0;
    // Virtual data processing
    virtual void processData() = 0;
};

// Derived class for an aircraft altimeter
class Altimeter : public Sensor {
public:
    // Altimeter data collection
    void gatherData() override {
        cout << "Gathering data from Altimeter Sensor..." << endl;
    }
    // Altimeter data processing
    void processData() override {
        cout << "Processing data from Altimeter Sensor..." << endl;
    }
};

// Derived class for an aircraft pitot tube
class PitotTube : public Sensor {
public:
    // Pitot tube data collection
    void gatherData() override {
        cout << "Gathering data from Pitot Tube Sensor..." << endl;
    }
    // Pitot tube data processing
    void processData() override {
        cout << "Processing data from Pitot Tube Sensor..." << endl;
    }
};

// Derived class for an aircraft gyroscope
class Gyroscope : public Sensor {
public:
    // Gyroscope data collection
    void gatherData() override {
        cout << "Gathering data from Gyroscope Sensor..." << endl;
    }
    // Gyroscope data processing
    void processData() override {
        cout << "Processing data from Gyroscope Sensor..." << endl;
    }
};