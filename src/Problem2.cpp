// Included to run
#include "HW2Problems.hpp"
// Everything else
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

const int Num_Bots = 5;
const int Exercise_Duration = 5; // Duration for task completion
mutex console_mutex;    // Mutex for console output (prevents lack of legibility)
mutex tools[Num_Bots];  // Mutexes for each tool (one robot can hold 2 tools at a given time)

// Robot lifecycle
void robotTask(int robotId) {
    // Tool ids go from 0,1,2,3,4 for left tool and 1,2,3,4,0 for right tool 
    // (6 tools total in between 5 robots, with mod for repeating)
    int leftTool = robotId; 
    int rightTool = (robotId + 1) % Num_Bots; 
    // Determine the order to lock to prevent deadlocks: 
    // lower number always gets chosen first (left side preferred)
    int firstTool = min(leftTool, rightTool); 
    int secondTool = max(leftTool, rightTool);

    // Log activation
    // Lock the console when outputting text so that the text is legible
    {
        lock_guard<mutex> lock(console_mutex);
        cout << "Robot " << robotId << " is active and reaching for the tools." << endl;
    }
    // Suppose it takes all robots 1 second to activate and reach for tools
    this_thread::sleep_for(chrono::seconds(1));
    
    // Grabbing of tools is employed by multilocking atomically with the lock function.
    // The lock command ensures that deadlocking doesn't happen. It waits until all arguments are satisfied
    lock(tools[firstTool], tools[secondTool]);
    // Proceed if both tools are grabbed..

    // Log performance with the tools
    // Using adopt lock because the tools are already locked. 
    lock_guard<mutex> lock1(tools[firstTool], adopt_lock);
    lock_guard<mutex> lock2(tools[secondTool], adopt_lock);
    {
        lock_guard<mutex> lock(console_mutex);
        cout << "Robot " << robotId << " has acquired tools and starts performing a task." << endl;
    }
    this_thread::sleep_for(chrono::seconds(Exercise_Duration)); // Sleep for 5 seconds
    {
        lock_guard<mutex> lock(console_mutex);
        cout << "Robot " << robotId << " has completed the task and returned the tools." << endl;
    }
    // Tools are automatically released when lock_guard goes out of scope
}

// Problem 2 function to be run in Main
void runProblem2() {
    cout << "################ Question 2 ###################" << endl;
    auto start_time = chrono::high_resolution_clock::now();
    // Create threads for each robot
    thread robots[Num_Bots];
    for (int i = 0; i < Num_Bots; i++) {
        robots[i] = thread(robotTask, i);
    }
    // No need for start flags like problem 3 because the robot locks the console and waits for 1 second before beginning.
    // This makes all robots start at the same time 
    // Join all robot threads
    for (int i = 0; i < Num_Bots; i++) {
        robots[i].join();
    }
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;
    cout << "Duration: " << duration.count() << " seconds." << endl;
    cout << "################ End of Question 2 ###################" << endl;
}
