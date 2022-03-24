#include "dispatcher.h"
#include <iostream>

const int NUM_PROC = 20; // number of processes

// generate random processes
void createRandomFile() {
    ofstream op;
    op.open("source/processes.txt");
    for (int i = 0; i < NUM_PROC; i++) {        
        op << i+100 << " " << rand() % 10 << " " << rand() % 20 << " " << rand() % 10 << " \n";
        //    PID             Arrival Time          CPU burst time        I/O burst time
    }
    op.close();
}

int main () {
    createRandomFile();
    Dispatcher dis;
    dis.simulate();
    cout << "hi";
}