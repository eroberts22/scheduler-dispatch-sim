#include "../headers/dispatcher.h"
#include <iostream>
#include <time.h>

const int NUM_PROC = 5; // number of processes

// generate random processes
void createRandomFile() {
    ofstream op;
    op.open("source/processes.txt");

    //op << 100 << " " << 1 << " " << 8 << " " << 0 << " \n"; 
    //op << 101 << " " << 4 << " " << 5 << " " << 0 << " \n"; 
    for (int i = 0; i < NUM_PROC; i++) {        
        op << i+100 << " " << rand() % 10 << " " << rand() % 9 + 1 << " " << rand() % 1 << " \n";
        //    PID             Arrival Time          CPU burst time        I/O burst time
    }
    op.close();
}

int main () {
    srand(time(NULL));
    createRandomFile();
    Dispatcher dis;
    dis.simulate();
}