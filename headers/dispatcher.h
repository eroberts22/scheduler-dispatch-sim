#pragma once
// Dispatcher.h
#include <deque>
#include <fstream>
#include <iostream>

#include "CPU.h"
#include "process.h"

class Dispatcher {
   private:
    // list of all processes
    deque<Process> all_processes;

    // the dispatcher manages 4 CPUs
    CPU CPU1;
    CPU CPU2;
    CPU CPU3;
    CPU CPU4;

    ofstream op; // writing to data file
    const int NUM_PROC = 5;  // number of processes


   public:
    Dispatcher() {}

    ~Dispatcher() {}

    // generate random processes
    void createRandomFile() {
        ofstream open;
        open.open("source/processes.txt");

        //op << 100 << " " << 1 << " " << 4 << " " << 0 << " \n";
        //op << 101 << " " << 1 << " " << 3 << " " << 0 << " \n";
        //op << 102 << " " << 1 << " " << 2 << " " << 0 << " \n";
        for (int i = 0; i < NUM_PROC; i++) {
            open << i + 100 << " " << rand() % 10 << " " << rand() % 9 + 1 << " " << rand() % 4 << " \n";
            //    PID             Arrival Time          CPU burst time          I/O burst time
        }
        open.close();
    }

    // read process information into dispatcher
    void readFile() {
        ifstream in("source/processes.txt");
        int p, a, c, i;
        while (in >> p >> a >> c >> i) {
            all_processes.push_back(Process(p, a, c, i));
        }
    }

    void simulate() {
        
        createRandomFile(); // create random file of processes
        readFile(); // reads processes into dispatcher

        // open data file
        op.open("data.csv");
        op << "test,avg.wait,avg.response,avg.turnaround\n";

        test_rr1();   // round robin q = 1
        test_rr4();   // round robin q = 4
        test_fcfs();  // first come first serve
        test_spn();   // shortest process next

        op.close();
    }

    void test_rr1() {
        cout << "ROUND ROBIN Q = 1\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.RR(1);
        CPU1.printCompleteProcesses();
        singleProcCalc("RR1");
        CPU1.clearAllProcesses();
    }

    void test_rr4() {
        cout << "ROUND ROBIN Q = 4\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.RR(4);
        CPU1.printCompleteProcesses();
        singleProcCalc("RR4");
        CPU1.clearAllProcesses();
    }

    void test_fcfs() {
        cout << "FIRST COME FIRST SERVE\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.FCFS();
        CPU1.printCompleteProcesses();
        singleProcCalc("FCFS");
        CPU1.clearAllProcesses();
    }

    void test_spn() {
        cout << "SHORTEST PROCESS NEXT\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.SPN();
        CPU1.printCompleteProcesses();
        singleProcCalc("SPN");
        CPU1.clearAllProcesses();
    }

    //////////////////////////////////////////////////////////////////////////
    //                            Calculations                              //
    //////////////////////////////////////////////////////////////////////////

    void singleProcCalc(string test) {
        int wait = 0;
        int resp = 0;
        int turn = 0;
        
        // avg wait time
        for (int x = 0; x < CPU1.getCompleteProcesses().size(); x++) {
            wait += CPU1.getCompleteProcesses().at(x).getWaitTime();
        }
        wait = wait / CPU1.getCompleteProcesses().size();

        // avg response time
        for (int i = 0; i < CPU1.getCompleteProcesses().size(); i++) {
            resp += CPU1.getCompleteProcesses().at(i).getResponseTime();
        }
        resp = resp / CPU1.getCompleteProcesses().size();

        // avg turnaround time
        for (int i = 0; i < CPU1.getCompleteProcesses().size(); i++) {
            turn += CPU1.getCompleteProcesses().at(i).getTurnTime();
        }
        turn = turn / CPU1.getCompleteProcesses().size();

        // file write
        op << test << "," << wait << "," << resp << "," << turn << "\n";

        // console output
        cout << test << " | Avg Wait: " << wait << " | Avg Response: " << resp << " | Avg Turnaround: " << turn << "\n\n";
    }
};