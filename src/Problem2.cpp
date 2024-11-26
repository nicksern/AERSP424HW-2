#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>
using namespace std;

const int Num_Bots = 5;
const int Max_Concurrent_Bots = 2;  // Max number of robots that can work simultaneously
const int Exercise_Duration = 5;          // Variable to measure duration of the exercise

mutex console_mutex;             // Mutex for synchronization
mutex semaphore_mutex;           // Mutex for tracking number of active robots
condition_variable cv;           // Conditional variable to control access of tools
int active_robots = 0;           // Counter for active robots

queue<int> robotQueue;           // Variable to directly manage order of robot activity

// Function for robot to complete task
void robotTask(int robotId) {
    {
        unique_lock<mutex> lock(semaphore_mutex);
        // Waiting for number of working robots to be less than 2
        cv.wait(lock, [robotId] {
            return active_robots < Max_Concurrent_Bots && !robotQueue.empty() && robotQueue.front() == robotId;
        });
        ++active_robots;              // Incrementing count of active robots
        robotQueue.pop();             // Removing the active robot from the tool queue
    }

    // Robots reaching for and grabbing tools
    {
        lock_guard<mutex> lock(console_mutex);
        // Output statement confirming the robot, with #, is going for the tools
        cout << "Robot " << robotId << " is reaching and grabbing the tools." << endl;
    }

    // Having timer wait 1 second for robot to acquire the tools
    this_thread::sleep_for(chrono::seconds(1)); 

    // Simulate performing the task
    {
        lock_guard<mutex> lock(console_mutex);
        cout << "Robot " << robotId << " has acquired tools and starts performing a task." << endl;
    }

    // Telling time to sleep for the 5 seconds required for the task and tool return
    this_thread::sleep_for(chrono::seconds(Exercise_Duration));

    // Robot has completed the task
    {
        lock_guard<mutex> lock(console_mutex);
        // Output message that the robot, with ID number, has completed the task and returned tools
        cout << "Robot " << robotId << " has completed the task and returned the tools." << endl;
    }

    // Release the semaphore and notify robots in the queue
    {
        unique_lock<mutex> lock(semaphore_mutex);
        --active_robots;              // Decrementing active robot count
        cv.notify_all();              // Notify robots in the queue
    }
}

int main() {
    // Output message for the start of question 2 
    cout << "################ Question 2 ###################" << endl;
    // Start the timer for duration of the exercise
    auto start_time = chrono::high_resolution_clock::now();

    // Create thread for the number of robots
    thread robots[Num_Bots];

    // Initializing the queue with robot IDs
    {
        lock_guard<mutex> lock(semaphore_mutex);
        // For loop to manually organize the robot queue
        for (int i = 0; i < Num_Bots; ++i) {
            robotQueue.push(i);
        }
    }

    // Creating robot threads
    for (int i = 0; i < Num_Bots; ++i) {
        robots[i] = thread(robotTask, i);
    }

    // Joining all robot threads
    for (int i = 0; i < Num_Bots; ++i) {
        robots[i].join();
    }

    // Calculating duration of the exercise
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    // Output message for exercise duration
    cout << "Duration : " << duration.count() << " seconds." << endl;
    // Output message for the end of question 2
    cout << "################ End of Question 2 ###################" << endl;

    // Ending main
    return 0;
}

