#include <iostream>
#include "Assembly.h"

using namespace std;

int main()
{
    CPUSimulator simulator;
    std::string filename;

    simulator.loadMemory("t.txt");

    simulator.run();

    simulator.displayContent();
    return 0;
}