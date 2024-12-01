// Included to run
#include "HW2Problems.hpp"
// Everything else
#include <iostream>                 
#include <thread>  
#include <mutex>  
#include <condition_variable> 
#include <chrono> 
using namespace std;

// Constants for simulation parameters
const int Max_TraffPatt = 3;        // Maximum aircraft allowed in the traffic pattern
const int Num_Aircraft = 10;        // Total number of aircraft attempting to land
const int Landing_Time = 1;         // Time (in seconds) required for landing

// Mutexes and condition variables for synchronization
mutex trafficPatternMutex;          // Protects access to trafficPatternCount
mutex atcMutex;                     // Protects access to ATC variables
condition_variable cv_atcAvailable; // Signals when ATC is available
mutex coutMutex;                    // Synchronizes console output

// Variables for ATC and traffic pattern management
int trafficPatternCount = 0;        // Current aircraft in the traffic pattern
int numWaitingPilots = 0;           // Pilots waiting to talk to ATC
bool atcBusy = false;               // Is ATC talking to a pilot
bool atcSleeping = true;            // Is ATC asleep

// Variables for synchronizing the start of all aircraft
mutex startMutex;                   // Protects startFlag
condition_variable startCV;         // Signals the start of simulation
bool startFlag = false;             // Indicates when to start

// Function executed by each aircraft thread
void aircraft(int aircraftId) {
    // ----- Wait until the start signal is given -----
    {
        unique_lock<mutex> lock(startMutex);           // Lock startMutex
        startCV.wait(lock, [] { return startFlag; });  // Wait until startFlag is true
    }

    // ----- Try to enter the traffic pattern -----
    {
        unique_lock<mutex> lock(trafficPatternMutex);  // Lock trafficPatternMutex
        if (trafficPatternCount >= Max_TraffPatt) {    // Check if traffic pattern is full
            // Traffic pattern is full; aircraft must divert
            {
                lock_guard<mutex> lock(coutMutex);
                cout << "Aircraft " << aircraftId << " is diverting to another airport. Traffic pattern full." << endl;
            }
            return;
        }
        // Space available; enter traffic pattern
        trafficPatternCount++; // Increment count
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Aircraft " << aircraftId << " has entered the traffic pattern." << endl;
        }
    }

    // ----- Try to communicate with ATC -----
    unique_lock<mutex> atcLock(atcMutex); // Lock atcMutex
    
    // ----- ATC Operation -----
    if (atcBusy) {
        // ATC is busy; wait in traffic pattern
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Aircraft " << aircraftId << " is waiting in the traffic pattern." << endl;
        }
        numWaitingPilots++; // Increment waiting pilots
        // Wait until ATC is available
        cv_atcAvailable.wait(atcLock, [] { return !atcBusy; });
        numWaitingPilots--; // Decrement after being notified
    }
    // ATC is available
    if (atcSleeping) {
        atcSleeping = false; // Wake up ATC
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "Aircraft " << aircraftId << " is waking up ATC." << endl;
        }
    }
    atcBusy = true; // ATC is now busy
    {
        lock_guard<mutex> lock(coutMutex);
        cout << "Aircraft " << aircraftId << " is communicating with ATC and cleared to land." << endl;
    }
    atcLock.unlock(); // Unlock atcMutex

    // ----- Simulate landing -----
    this_thread::sleep_for(chrono::seconds(Landing_Time));  // Simulate landing time
    // After landing
    atcLock.lock(); // Lock atcMutex
    atcBusy = false; // ATC is no longer busy
    {
        lock_guard<mutex> lock(coutMutex);
        cout << "Aircraft " << aircraftId << " has landed." << endl;
    }
    if (numWaitingPilots == 0) { // No pilots waiting
        atcSleeping = true; // ATC goes back to sleep
        {
            lock_guard<mutex> lock(coutMutex);
            cout << "ATC is going back to sleep." << endl;
        }
    }
    cv_atcAvailable.notify_one(); // Notify one waiting pilot
    atcLock.unlock(); // Unlock atcMutex
    {
        unique_lock<mutex> lock(trafficPatternMutex);  // Lock trafficPatternMutex
        trafficPatternCount--;  // Decrement traffic pattern count
    }
}

// Problem 3 function to be run in Main
void runProblem3() {
    cout << "################ Question 3 ###################" << endl;
    auto start_time = chrono::high_resolution_clock::now();
    // Create aircraft threads
    thread aircraftThreads[Num_Aircraft];
    for (int i = 0; i < Num_Aircraft; ++i) {
        aircraftThreads[i] = thread(aircraft, i);
    }
    // Release all aircraft at once (10 airplanes are coming at ONCE)
    // We could have alternatively done the same as the robot problem in problem 2, but
    // then we need to introduce some sleep as the consoles are all locked.
    {
        lock_guard<mutex> lock(startMutex);            // Lock startMutex
        startFlag = true;                              // Set startFlag to true
    }
    startCV.notify_all();                              // Notify all aircraft threads
    // Join threads
    for (int i = 0; i < Num_Aircraft; ++i) {
        aircraftThreads[i].join();
    }
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;
    cout << "Duration: " << duration.count() << " seconds." << endl;
    cout << "################ End of Question 3 ###################" << endl;
}
