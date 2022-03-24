#include "../headers/dispatcher.h"
#include <iostream>
#include <time.h>

const int NUM_PROC = 4; // number of processes

// generate random processes
void createRandomFile() {
    ofstream op;
    op.open("source/processes.txt");
    for (int i = 0; i < NUM_PROC; i++) {        
        op << i+100 << " " << rand() % 10 << " " << rand() % 9 + 1 << " " << rand() % 5 << " \n";
        //    PID             Arrival Time          CPU burst time        I/O burst time
    }
    op.close();
}

int main () {
    srand(time(NULL));
    createRandomFile();
    Dispatcher dis;
    dis.simulate();
    cout << "hi";
}