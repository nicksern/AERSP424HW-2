#include "HW2Problems.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp> 
// Platform-specific includes to get the executable path
#if defined(_WIN32)
    #include <windows.h>
    #include <limits.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#endif
// For the sake of practice, we decided to use the OpenGL Library directly for part 2 of problem 4. This resulted in excessively long code,
// but proved to be a useful exercise.
using namespace std;

// Define a structure to represent a 2D point with x and y coordinates
struct Point 
{
    float x, y;
};

// ------------------------------------------------------------------------------------------
// AI GENERATED CODE INCOMING
// Approval: https://psu.instructure.com/courses/2333977/discussion_topics/16342893 
// ------------------------------------------------------------------------------------------
// NOTICE: THE FOLLOWING FUNCTIONS WERE MADE POSSIBLE WITH THE USE OF AI.
// THIS WAS NECESSARY IN FINDING SETTING UP SYSTEM COMPATIBILITY AND FILE LOCATIONS (couldn't figure out how to access the file...)
// WE USED CHATGPT: https://chatgpt.com/
// We've also asked the AI for help in commenting these sections.
// ..
// Function to get the path of the executable
string getExecutablePath() { // AI generated/support
    char buffer[PATH_MAX]; // Buffer (temporary char) to store the path
#if defined(_WIN32)
    DWORD length = GetModuleFileNameA(NULL, buffer, PATH_MAX);
    if (length == 0 || length == PATH_MAX) {
        cerr << "Error getting executable path on Windows." << endl;
        exit(EXIT_FAILURE);
    }
#elif defined(__linux__)
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (length == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }
    buffer[length] = '\0'; // Null-terminate the string
#elif defined(__APPLE__)
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        cerr << "Buffer too small; need size " << size << endl;
        exit(EXIT_FAILURE);
    }
#else
    #error "Unsupported platform"
#endif
    string fullPath(buffer); // Convert the buffer to a string
    // Remove the executable name to get the directory
    size_t pos = fullPath.find_last_of("/\\");
    return fullPath.substr(0, pos);
}
// Function to read points from a CSV file
vector<Point> readCSV(const string &filename) { // AI generated/support
    vector<Point> points; // Vector to store the points
    ifstream file(filename); // Open the file
    if (!file.is_open()) {
        // If the file can't be opened, print an error and return an empty vector
        cerr << "Failed to open file: " << filename << endl;
        perror("Error: Failed to open file"); // error function
        return points;
    }
    string line;
    // Skip the header line in the CSV file
    getline(file, line);
    // Read each line from the CSV file
    while (getline(file, line)) {
        istringstream ss(line); // Create a string stream for parsing
        string timeStr, xStr, yStr;
        // Parse the time, x, and y values separated by commas
        if (getline(ss, timeStr, ',') &&
            getline(ss, xStr, ',') &&
            getline(ss, yStr, ',')) {
            try {
                // Convert the x and y strings to floats and create a Point
                Point point = {stof(xStr), stof(yStr)};
                points.push_back(point); // Add the point to the vector
            } catch (const exception &e) {
                // If there's an error in parsing, print an error message
                cerr << "Error parsing line: " << line << "\n"
                            << "Exception: " << e.what() << endl;
            }
        }
    }
    file.close(); // Close the file
    return points; // Return the vector of points
}
// END OF AI GENERATED CODE
// ------------------------------------------------------------------------------------------

// ----- 
// The following functions were made possible with the 0260-graphic-programming.pdf slides...
// ----- 

// Draw the letter 'X'
void drawLetterX(float x, float y, float size) 
{
    glBegin(GL_LINES);
    // First diagonal of 'X'
    glVertex2f(x - size, y - size); 
    glVertex2f(x + size, y + size);
    // Second diagonal of 'X'
    glVertex2f(x - size, y + size); 
    glVertex2f(x + size, y - size); 
    glEnd();
}

// Draw the letter 'Y'
void drawLetterY(float x, float y, float size) 
{
    glBegin(GL_LINES);
    // Left branch of 'Y'
    glVertex2f(x, y);
    glVertex2f(x - size * 0.7f, y + size);
    // Right branch of 'Y'
    glVertex2f(x, y); 
    glVertex2f(x + size * 0.7f, y + size);
    // Stem of 'Y'
    glVertex2f(x, y); 
    glVertex2f(x, y - size);
    glEnd();
}

// Callback function for window resizing
void framebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
    // Match the new window dimensions
    glViewport(0, 0, width, height);
}

// Function to draw the graph, axes, and labels
void drawGraph(const vector<Point>& p) 
{
    glClear(GL_COLOR_BUFFER_BIT);
    // Variables to hold the minimum and maximum x and y values
    float minX = p[0].x, maxX = p[0].x;
    float minY = p[0].y, maxY = p[0].y;
    
    // Find min and max values for normalization
    for (const auto& pt : p) 
    {
        if (pt.x < minX) 
        minX = pt.x; 
        if (pt.x > maxX) 
        maxX = pt.x; 
        if (pt.y < minY) 
        minY = pt.y; 
        if (pt.y > maxY) 
        maxY = pt.y;
    }
    // Normalize points to fit in the range [-1, 1]
    vector<Point> normPts;
    for (const auto& pt : p) 
    {
        float normX = (2.0f * (pt.x-minX) / (maxX-minX))-1.0f;
        float normY = (2.0f * (pt.y-minY) / (maxY-minY))-1.0f;
        // Add the normalized point to the vector
        normPts.push_back({normX, normY});
    }

    // Draw the axes
    glLineWidth(2.0f); 
    glBegin(GL_LINES);
    // X-axis in red color
    glColor3f(1.0f, 0.0f, 0.0f); 
    glVertex2f(-1.0f, 0.0f); 
    glVertex2f(1.0f, 0.0f); 
    // Y-axis in green color
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.0f, -1.0f); 
    glVertex2f(0.0f, 1.0f);  
    glEnd(); 

    // Draw ticks on the axes
    glLineWidth(1.0f); 
    glBegin(GL_LINES); 
    for (float i = -1.0f; i <= 1.01f; i += 0.2f)
    {
        // X-axis ticks in red color
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex2f(i, -0.02f); 
        glVertex2f(i, 0.02f);
        // Y-axis ticks in green color
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex2f(-0.02f, i); 
        glVertex2f(0.02f, i); 
    }
    glEnd();

    // Draw the graph line
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for the plot
    glLineWidth(2.0f); 
    glBegin(GL_LINE_STRIP);
    for (const auto& pt : normPts) 
    {
        // Add each normalized point as a vertex
        glVertex2f(pt.x, pt.y);
    }
    glEnd();

    float labelSize = 0.03f;
    // Set color to white for labels
    glColor3f(1.0f, 1.0f, 1.0f);
    // Draw 'X' label at the end of the X-axis
    drawLetterX(0.9f, -0.08f, labelSize);
    // Draw 'Y' label at the end of the Y-axis
    drawLetterY(-0.08f, 0.9f, labelSize);
}

void runProblem4p2()
{
    cout << "################ Question 4.2 ###################" << endl;
    {
    // Get the executable's directory
    string exePath = getExecutablePath(); // Function generated with AI (see above)
    // Construct the path to the CSV file 'timedata.csv'
    string csvPath = exePath + "/timedata.csv"; // Name of file
    cout << "Executable Directory: " << exePath << endl;
    cout << "CSV Path: " << csvPath << endl;
    // Read the points from the CSV file
    vector<Point> points = readCSV(csvPath); // Function generated with AI (see above)
    if (points.empty()) // If failure
    {
        cerr << "No data to plot!" << endl;
    }

    // Initialize GLFW
    if (!glfwInit()) // If failure
    {
        cerr << "Failed to initialize GLFW" << endl;
    }
    // GLFW window
    GLFWwindow *window = glfwCreateWindow(800, 800, "2D Graph", nullptr, nullptr);
    if (!window) // If failure
    { 
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) 
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        drawGraph(points);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Destroy the window
    glfwDestroyWindow(window);
    glfwTerminate();
    }
    cout << "################ End of Question 4.2 ###################" << endl;
}
