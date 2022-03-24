#pragma once
// Dispatcher.h
#include <fstream>
#include <iostream>
#include "CPU.h"
#include "process.h"

class Dispatcher {
private:
    // list of all processes
    vector<Process> all_processes;

    // the dispatcher manages 4 CPUs
    CPU CPU1;
    CPU CPU2;
    CPU CPU3;
    CPU CPU4;
public:
    Dispatcher() {
       readFile();
    }

    ~Dispatcher() {}

    // read process information into dispatcher
    void readFile() {
        ifstream in("source/processes.txt");
        int p, a, c, i;
        while (in >> p >> a >> c >> i) {
            all_processes.push_back(Process(p,a,c,i));
        }    
    }

    void simulate() {
        //test_rr1();   // round robin q = 1
        //test_rr4();   // round robin q = 4
        test_fcfs();  // first come first serve
        //test_spn();   // shortest process next

    }

    void test_rr1() {
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.printAllProcesses();
        CPU1.RR(1);
        CPU1.clearAllProcesses();
    }

    void test_rr4() {
        CPU1.clearAllProcesses(); 
        CPU1.setProcesses(all_processes);
        CPU1.printAllProcesses();
        CPU1.RR(4);
        CPU1.clearAllProcesses();
    }

    void test_fcfs() {
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.printAllProcesses();
        CPU1.FCFS();
        CPU1.printCompleteProcesses();
        CPU1.clearAllProcesses();
    }

    void test_spn() {
        CPU1.clearAllProcesses(); 
        CPU1.setProcesses(all_processes);
        CPU1.printAllProcesses();
        CPU1.SPN();
        CPU1.clearAllProcesses();
    }

};