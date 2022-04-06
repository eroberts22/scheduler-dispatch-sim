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

    ofstream op; // writing to data file
    const int NUM_PROC = 20;  // number of processes

   public:
    Dispatcher() {}
    ~Dispatcher() {}

    // generate random processes
    void createRandomFile() {
        ofstream open;
        open.open("source/processes.txt");

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
        test_two_cpu_fcfs();  // two cores fcfs
        test_two_cpu_spn();   // two cores spn

        op.close();
    }

    pair<deque<Process>,deque<Process>> splitProcesses() {
        deque<Process> temp1;
        deque<Process> temp2;
        for (int i = 0; i < all_processes.size(); i++) {
            if (i % 2 == 0) {
                temp1.push_back(all_processes.at(i));
            } else {
                temp2.push_back(all_processes.at(i));
            }
        }

        return pair<deque<Process>,deque<Process>>(temp1,temp2);
    }

    void test_rr1() {
        cout << "ROUND ROBIN Q = 1\n\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.RR(1);
        CPU1.printCompleteProcesses();
        singleCPUCalc("RR1");
        CPU1.clearAllProcesses();
    }

    void test_rr4() {
        cout << "ROUND ROBIN Q = 4\n\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.RR(4);
        CPU1.printCompleteProcesses();
        singleCPUCalc("RR4");
        CPU1.clearAllProcesses();
    }

    void test_fcfs() {
        cout << "FIRST COME FIRST SERVE\n\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.FCFS();
        CPU1.printCompleteProcesses();
        singleCPUCalc("FCFS");
        CPU1.clearAllProcesses();
    }

    void test_spn() {
        cout << "SHORTEST PROCESS NEXT\n\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.SPN();
        CPU1.printCompleteProcesses();
        singleCPUCalc("SPN");
        CPU1.clearAllProcesses();
    }

    void test_two_cpu_fcfs() {
        cout << "MULTICORE FCFS - TWO CORES\n\n";
        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();

        pair<deque<Process>,deque<Process>> split_processes = splitProcesses();

        CPU1.setProcesses(split_processes.first);
        CPU2.setProcesses(split_processes.second);

        CPU1.FCFS();
        CPU2.FCFS();

        cout << "Processor 1\n";
        CPU1.printCompleteProcesses();
        cout << "Processor 2\n";
        CPU2.printCompleteProcesses();
        
        twoCPUCalc("MULT.FCFS");

        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();
    }

    void test_two_cpu_spn() {
        cout << "MULTICORE SPN - TWO CORES\n\n";
        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();

        pair<deque<Process>,deque<Process>> split_processes = splitProcesses();

        CPU1.setProcesses(split_processes.first);
        CPU2.setProcesses(split_processes.second);

        CPU1.SPN();
        CPU2.SPN();

        cout << "Processor 1\n";
        CPU1.printCompleteProcesses();
        cout << "Processor 2\n";
        CPU2.printCompleteProcesses();
        
        twoCPUCalc("MULT.SPN");

        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();
    }

    //////////////////////////////////////////////////////////////////////////
    //                            Calculations                              //
    //////////////////////////////////////////////////////////////////////////

    void singleCPUCalc(string test) {
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

    void twoCPUCalc(string test) {
        int wait1 = 0;
        int wait2 = 0;
        int resp1 = 0;
        int resp2 = 0;
        int turn1 = 0;
        int turn2 = 0;
        int wait_a = 0;
        int resp_a = 0;
        int turn_a = 0;
        
        // avg wait time
        for (int x = 0; x < CPU1.getCompleteProcesses().size(); x++) {
            wait1 += CPU1.getCompleteProcesses().at(x).getWaitTime();
        }
        wait1 = wait1 / CPU1.getCompleteProcesses().size();

        // avg response time
        for (int i = 0; i < CPU1.getCompleteProcesses().size(); i++) {
            resp1 += CPU1.getCompleteProcesses().at(i).getResponseTime();
        }
        resp1 = resp1 / CPU1.getCompleteProcesses().size();

        // avg turnaround time
        for (int i = 0; i < CPU1.getCompleteProcesses().size(); i++) {
            turn1 += CPU1.getCompleteProcesses().at(i).getTurnTime();
        }
        turn1 = turn1 / CPU1.getCompleteProcesses().size();

        // avg wait time
        for (int x = 0; x < CPU2.getCompleteProcesses().size(); x++) {
            wait2 += CPU2.getCompleteProcesses().at(x).getWaitTime();
        }
        wait2 = wait2 / CPU2.getCompleteProcesses().size();

        // avg response time
        for (int i = 0; i < CPU2.getCompleteProcesses().size(); i++) {
            resp2 += CPU2.getCompleteProcesses().at(i).getResponseTime();
        }
        resp2 = resp2 / CPU2.getCompleteProcesses().size();

        // avg turnaround time
        for (int i = 0; i < CPU2.getCompleteProcesses().size(); i++) {
            turn2 += CPU2.getCompleteProcesses().at(i).getTurnTime();
        }
        turn2 = turn2 / CPU2.getCompleteProcesses().size();

        wait_a = (wait1 + wait2)/2;
        resp_a = (resp1 + resp2)/2;
        turn_a = (turn1 + turn2)/2;

        // file write
        op << test << ".TWO," << wait_a << "," << resp_a << "," << turn_a << "\n";
        
        // console output
        cout << test << " CPU 1 | Avg Wait: " << wait1 << " | Avg Response: " << resp1 << " | Avg Turnaround: " << turn1 << "\n";
        cout << test << " CPU 2 | Avg Wait: " << wait2 << " | Avg Response: " << resp2 << " | Avg Turnaround: " << turn2 << "\n";
        cout << test << " AVG   | Avg Wait: " << wait_a << " | Avg Response: " << resp_a << " | Avg Turnaround: " << turn_a << "\n\n";
    }
};