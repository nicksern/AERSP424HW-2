// Run all the problems
#include "HW2Problems.hpp"
#include <iostream>
using namespace std;

int main() {
    runProblem1();
    runProblem2();
    runProblem3();
    cout << "In case something goes wrong with 4.1, 4.2 runs first..." << endl;
    runProblem4p2();
    runProblem4();
    return 0;
}