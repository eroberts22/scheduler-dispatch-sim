#pragma once
// CPU.h
#include <vector>
#include <queue>
#include <deque>
#include <algorithm>
#include "process.h"
using namespace std;

class CPU {
private:
    // all processes assigned to CPU
    deque<Process> cpu_processes;
    int num_proc; // track number of total processes
    int incrementor = 0; // used to increment through cpu processes

    // queues for shuffling processes between CPU and I/O
    deque<Process> IOQ;        // I/O operation queue
    deque<Process> CPUQ;       // CPU operation queue, element at front is the running process
    
    // completed processes
    // will show order they were completed in
    deque<Process> complete;

    int clock = 0; // time in system

public:

    // initialize CPU without processes
    CPU() {}

    // initialize CPU with processes
    CPU(deque<Process> v) {
        setProcesses(v);
        num_proc = v.size();
    }

    deque<Process> getProcesses() {
        return cpu_processes;
    }

    void setProcesses(deque<Process> v) {
        // sort the processes by arrival time
        sort(v.begin(), v.end()); 
        this->cpu_processes = v;
        num_proc = v.size();
    }

    void addProcess(Process p) {
        cpu_processes.push_back(p);
    }

    // clear all processes from CPU, reset clock
    void clearAllProcesses() {
        cpu_processes.clear();
        IOQ.clear();
        CPUQ.clear();
        complete.clear();
        clock = 0;
        incrementor = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //                         Printing Functions                           //
    //////////////////////////////////////////////////////////////////////////   

    // print all processes being handled by CPU
    void printAllProcesses() {
        cout << "PID |Arr_t|CPU_t|IO_t |Wait_t|Resp_t|Turn_t\n";
        cout << "----|-----|-----|-----|------|------|------\n";
        for (int i = 0; i < cpu_processes.size(); i++) {
            cout << cpu_processes.at(i).getAllInfo() << endl;
        }
        cout << endl;
    }

    // print all processes completed by CPU
    void printCompleteProcesses() {
        cout << "PID |Arr_t|CPU_t|IO_t |Wait_t|Resp_t|Turn_t\n";
        cout << "----|-----|-----|-----|------|------|------\n";
        for (int i = 0; i < complete.size(); i++) {
            cout << complete.at(i).getAllInfo() << endl;
        }
        cout << endl;
    }

    //////////////////////////////////////////////////////////////////////////
    //                          Clock Utility                               //
    //////////////////////////////////////////////////////////////////////////
    
    void checkProcesses() {
        // check if any processes should be pushed at clock
        while (true) {
            if(incrementor < num_proc && cpu_processes.at(incrementor).getArrivalTime() == clock) {
                CPUQ.push_back(cpu_processes.at(incrementor));
                incrementor++;
            } else {
                break;
            }
        }
        if (CPUQ.empty() && IOQ.empty() && !(complete.size()==num_proc)) {
            ClockInc();
        }
    }

    // increment clock and add processes
    void ClockInc() {
        // increment clock
        clock++;
        // check if any processes need to be moved to cpu queue
        if(incrementor != num_proc) {
            checkProcesses();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //                       First Come First Serve                         //
    //////////////////////////////////////////////////////////////////////////

    // handling the CPU queue for FCFS
    void FCFS_CPU() {
        while(!CPUQ.empty()) {            
            // if io time is greater than cpu time
            if (CPUQ.front().move_to_IO()) { // swap to io queue
                IOQ.push_back(CPUQ.front()); // move process to IO queue
                CPUQ.pop_front(); // remove process from CPU queue
            } else { // keep running cpu queue
                
                // if both cpu and io times are 0
                // move them to complete queue
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    complete.push_back(CPUQ.front());
                    CPUQ.pop_front();
                }

                CPUQ.front().decCPU(); // decrease cpu burst time
                
                CPUQ.front().incResponse(); // inc response time

                // increment wait time for processes in CPUQ, except front
                for (int i = 1; i < CPUQ.size(); i++) {
                    CPUQ.at(i).incWaitT();
                }
            }   
            ClockInc();
        }
        if(!IOQ.empty()) { // if io queue is not empty
            FCFS_IO();
        }   
    }

    // handling the IO queue for FCFS
    void FCFS_IO() {
        // while io queue is not empty
        while(!IOQ.empty()) {
            IOQ.front().decIO(); // decrease io burst time

            // increment wait time for processes in IOQ
        
            for (int i = 0; i < IOQ.size(); i++) {
                IOQ.at(i).incWaitT();
            }

            if (IOQ.front().getIOBurstTime() == 0) { // if io is done, move back to cpu
                CPUQ.push_back(IOQ.front());
                IOQ.pop_front();
            }
            ClockInc();
        }
        if (!CPUQ.empty()) {
            FCFS_CPU(); // return to CPU
        }
    }

    void FCFS() {
        checkProcesses(); // check processes at clock 0
        FCFS_CPU(); // start by running through cpu queue
    }

    //////////////////////////////////////////////////////////////////////////
    //                             Round Robin                              //
    //////////////////////////////////////////////////////////////////////////

    void RR_CPU(int q) {
        int counter = 0;
        while(!CPUQ.empty()) {            
            // if io time is greater than cpu time
            if (CPUQ.front().move_to_IO()) { // swap to io queue
                IOQ.push_back(CPUQ.front()); // move process to IO queue
                CPUQ.pop_front(); // remove process from CPU queue
                counter = 0; // if process moves to IO queue, reset to 0
            } else { // keep running cpu queue
                
                CPUQ.front().decCPU(); // decrease cpu burst time
                //cout << "process "<< CPUQ.front().getPID() << endl;
                CPUQ.front().incResponse(); // inc response time

                // increment wait time for processes in CPUQ, except front
                for (int i = 1; i < CPUQ.size(); i++) {
                    CPUQ.at(i).incWaitT();
                }
                // if both cpu and io times are 0
                // move them to complete queue
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    complete.push_back(CPUQ.front());
                    CPUQ.pop_front();
                    counter = 0; // if process finishes before time quantum, reset to 0
                } else {
                    // counter for round robin
                    if (counter >= q) {
                        counter = 0;
                        // pop process to back of queue
                        CPUQ.push_back(CPUQ.front());
                        CPUQ.pop_front();
                    }
                    counter++; 
                }
            }   
            ClockInc();
        }
        if(!IOQ.empty()) { // if io queue is not empty
            RR_IO(q);
        }   
    }

    void RR_IO(int q) {
        while(!IOQ.empty()) {
            IOQ.front().decIO(); // decrease io burst time

            // increment wait time for processes in IOQ
            for (int i = 0; i < IOQ.size(); i++) {
                IOQ.at(i).incWaitT();
            }

            if (IOQ.front().getIOBurstTime() == 0) { // if io is done, move back to cpu
                CPUQ.push_back(IOQ.front());
                IOQ.pop_front();
            }
            ClockInc();
        }
        if (!CPUQ.empty()) {
            RR_CPU(q); // return to CPU
        }
    }

    void RR(int q) {
        checkProcesses();
        RR_CPU(q);
    }

    //////////////////////////////////////////////////////////////////////////
    //                       Shortest Process Next                          //
    //////////////////////////////////////////////////////////////////////////
    void SPN() {}

};
