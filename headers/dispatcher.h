#pragma once
// Dispatcher.h
#include <deque>
#include <fstream>
#include <iostream>

#include "CPU.h"
#include "process.h"

class Dispatcher {
   private:
    // number of processes
    const int NUM_PROC = 30; 
    const int NUM_TEST = 5; 
    int test_num = 0;

    // list of all processes
    deque<Process> all_processes;

    // the dispatcher manages 2 CPUs
    CPU CPU1;
    CPU CPU2;

    ofstream op_wait;
    ofstream op_resp;
    ofstream op_turn;
    ofstream op_thru;
    ofstream op_speed;

   public:
    Dispatcher() {}
    ~Dispatcher() {}

    // generate random processes
    void createRandomFile() {
        ofstream open;
        open.open("source/processes.txt");

        for (int i = 0; i < NUM_PROC; i++) {
            open << i + 100 << " " << rand() % 30 << " " << rand() % 20 + 1 << " " << rand() % 5 << " \n";
            //    PID             Arrival Time          CPU burst time           I/O burst time
        }
        open.close();
    }

    // read process information into dispatcher
    void readFile() {
        all_processes.clear(); // make sure there are no old processes
        ifstream in("source/processes.txt");
        int p, a, c, i;
        while (in >> p >> a >> c >> i) {
            all_processes.push_back(Process(p, a, c, i));
        }
    }

    // split processes
    // every other process goes to each queue
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

    // split processes
    // one queue has lower io times
    // other queue has higher io times
    pair<deque<Process>,deque<Process>> splitProcessesDist() {
        deque<Process> temp1;
        deque<Process> temp2;
        for (int i = 0; i < all_processes.size(); i++) {
            if (all_processes.at(i).getOIOBurstTime() == 0) {
                temp1.push_back(all_processes.at(i));
            } else {
                temp2.push_back(all_processes.at(i));
            }
        }

        return pair<deque<Process>,deque<Process>>(temp1,temp2);
    }

    void test() {
        createRandomFile(); // create random file of processes
        readFile(); // reads processes into dispatcher

        test_rr1();                 // round robin q = 1
        test_rr4();                 // round robin q = 4
        test_rr8();                 // round robin q = 8
        test_fcfs();                // first come first serve
        test_two_cpu_fcfs();        // two cores fcfs
        test_two_cpu_fcfs_dist();   // two cores fcfs cpu and io distribution
        test_spn();                 // shortest process next
        test_two_cpu_spn();         // two cores spn

    }

    void simulate() {
        // open data file
        op_wait.open("data_wait.csv");
        op_resp.open("data_resp.csv");
        op_turn.open("data_turn.csv");
        op_thru.open("data_thru.csv");
        //op << "test.num,test,avg.wait,avg.response,avg.turnaround,throughput\n";
        op_wait << "test,RR1,RR4,RR8,FCFS,FCFS.MULT,FCFS.MULT.DIST,SPN,SPN.MULT\n";
        op_resp << "test,RR1,RR4,RR8,FCFS,FCFS.MULT,FCFS.MULT.DIST,SPN,SPN.MULT\n";
        op_turn << "test,RR1,RR4,RR8,FCFS,FCFS.MULT,FCFS.MULT.DIST,SPN,SPN.MULT\n";
        op_thru << "test,RR1,RR4,RR8,FCFS,FCFS.MULT,FCFS.MULT.DIST,SPN,SPN.MULT\n";
        for (int i = 0; i < NUM_TEST; i++) {
            test_num = i+1;
            op_wait << test_num << ",";
            op_resp << test_num << ",";
            op_turn << test_num << ",";
            op_thru << test_num << ",";
            cout << "----------------------------------- TEST " << i+1 << " -----------------------------------\n";
            test();
            op_wait << "\n";
            op_resp << "\n";
            op_turn << "\n";
            op_thru << "\n";
        }
        
        op_wait.close();
        op_resp.close();
        op_turn.close();
        op_thru.close();
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

    void test_rr8() {
        cout << "ROUND ROBIN Q = 8\n\n";
        CPU1.clearAllProcesses();
        CPU1.setProcesses(all_processes);
        CPU1.RR(8);
        CPU1.printCompleteProcesses();
        singleCPUCalc("RR8");
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
        
        twoCPUCalc("FCFS.MULT");

        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();
    }

    void test_two_cpu_fcfs_dist() {
        cout << "MULTICORE FCFS - TWO CORES - DIST\n\n";
        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();

        pair<deque<Process>,deque<Process>> split_processes = splitProcessesDist();

        CPU1.setProcesses(split_processes.first);
        CPU2.setProcesses(split_processes.second);

        CPU1.FCFS();
        CPU2.FCFS();

        cout << "Processor 1\n";
        CPU1.printCompleteProcesses();
        cout << "Processor 2\n";
        CPU2.printCompleteProcesses();
        
        twoCPUCalc("FCFS.MULT.DIST");

        CPU1.clearAllProcesses();
        CPU2.clearAllProcesses();
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
        
        twoCPUCalc("SPN.MULT");

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
        int thru = 0;

        for (int x = 0; x < CPU1.getCompleteProcesses().size(); x++) {
            wait += CPU1.getCompleteProcesses().at(x).getWaitTime();
            resp += CPU1.getCompleteProcesses().at(x).getResponseTime();
            turn += CPU1.getCompleteProcesses().at(x).getTurnTime();
        }
        wait = wait / CPU1.getCompleteProcesses().size();
        resp = resp / CPU1.getCompleteProcesses().size();
        turn = turn / CPU1.getCompleteProcesses().size();

        thru = turn;

        // file write
        //op << test_num << "," << test << "," << wait << "," << resp << "," << turn << "," << thru << "\n";
        op_wait << wait << ",";
        op_resp << resp << ",";
        op_turn << turn << ",";
        op_thru << thru << ",";
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
        int thru = 0;
        
        for (int x = 0; x < CPU1.getCompleteProcesses().size(); x++) {
            wait1 += CPU1.getCompleteProcesses().at(x).getWaitTime();
            resp1 += CPU1.getCompleteProcesses().at(x).getResponseTime();
            turn1 += CPU1.getCompleteProcesses().at(x).getTurnTime();
        }

        wait1 = wait1 / CPU1.getCompleteProcesses().size();
        resp1 = resp1 / CPU1.getCompleteProcesses().size();
        turn1 = turn1 / CPU1.getCompleteProcesses().size();

        for (int x = 0; x < CPU2.getCompleteProcesses().size(); x++) {
            wait2 += CPU2.getCompleteProcesses().at(x).getWaitTime();
            resp2 += CPU2.getCompleteProcesses().at(x).getResponseTime();
            turn2 += CPU2.getCompleteProcesses().at(x).getTurnTime();
        }

        wait2 = wait2 / CPU2.getCompleteProcesses().size();
        resp2 = resp2 / CPU2.getCompleteProcesses().size();
        turn2 = turn2 / CPU2.getCompleteProcesses().size();

        wait_a = (wait1 + wait2)/2;
        resp_a = (resp1 + resp2)/2;
        turn_a = (turn1 + turn2)/2;

        thru = turn_a;

        // file write
        //op << test_num << "," << test << "," << wait_a << "," << resp_a << "," << turn_a << "," << thru << "\n";
        op_wait << wait_a << ",";
        op_resp << resp_a << ",";
        op_turn << turn_a << ",";
        op_thru << thru << ",";

        // console output
        cout << test << " CPU 1 | Avg Wait: " << wait1 << " | Avg Response: " << resp1 << " | Avg Turnaround: " << turn1 << "\n";
        cout << test << " CPU 2 | Avg Wait: " << wait2 << " | Avg Response: " << resp2 << " | Avg Turnaround: " << turn2 << "\n";
        cout << test << " AVG   | Avg Wait: " << wait_a << " | Avg Response: " << resp_a << " | Avg Turnaround: " << turn_a << "\n\n";
    }
};