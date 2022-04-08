#pragma once
// CPU.h
#include <algorithm>
#include <deque>
#include <queue>
#include <vector>

#include "process.h"
using namespace std;

class CPU {
   private:
    
    deque<Process> cpu_processes; // all processes assigned to CPU
    deque<Process> IOQ;           // I/O operation queue
    deque<Process> CPUQ;          // CPU operation queue, element at front is the running process
    deque<Process> complete;      // completed processes

    int clock = 0;                // time in system
    int num_proc;                 // track number of total processes
    int incrementor = 0;          // used to increment through cpu processes

   public:
    // initialize CPU without processes
    CPU() {}

    // initialize CPU with processes
    CPU(deque<Process> v) {
        setProcesses(v);
        num_proc = v.size();
    }

    deque<Process> getProcesses() { return cpu_processes; }

    deque<Process> getCompleteProcesses() { return complete; }

    void setProcesses(deque<Process> v) {
        // sort the processes by arrival time
        sort(v.begin(), v.end());
        this->cpu_processes = v;
        num_proc = v.size();
    }

    // clear all processes from CPU, reset clock
    void clearAllProcesses() {
        cpu_processes.clear();
        IOQ.clear();
        CPUQ.clear();
        complete.clear();
        clock = 0;
        num_proc = 0;
        incrementor = 0;
    }

    //////////////////////////////////////////////////////////////////////////
    //                         Printing Functions                           //
    //////////////////////////////////////////////////////////////////////////

    // print all processes being handled by CPU
    void printAllProcesses() {
        cout << "PID|Arr_t|CPU_t|IO_t |Wait_t|Resp_t|Turn_t\n";
        cout << "---|-----|-----|-----|------|------|------\n";
        for (int i = 0; i < cpu_processes.size(); i++) {
            //cout << cpu_processes.at(i).getAllInfo() << endl;
            cpu_processes.at(i).getAllInfo();
        }
        cout << endl;
    }

    // print all processes completed by CPU
    void printCompleteProcesses() {
        cout << "PID|Arr_t|CPU_t|IO_t |Wait_t|Resp_t|Turn_t\n";
        cout << "---|-----|-----|-----|------|------|------\n";
        for (int i = 0; i < complete.size(); i++) {
            //cout << complete.at(i).getAllInfo() << endl;
            complete.at(i).getAllInfo();
        }
        cout << endl;
    }

    //////////////////////////////////////////////////////////////////////////
    //                          Clock Utilities                             //
    //////////////////////////////////////////////////////////////////////////

    // check processes' arrival time
    void checkProcesses() {
        // check if any processes should be pushed at clock
        while (true) {
            if (incrementor < num_proc && cpu_processes.at(incrementor).getArrivalTime() == clock) {
                CPUQ.push_back(cpu_processes.at(incrementor));
                incrementor++;
            } else {
                break;
            }
        }
        if (CPUQ.empty() && IOQ.empty() && !(complete.size() == num_proc)) {
            ClockInc();
        }
    }

    // increment clock
    void ClockInc() {
        // increment clock
        clock++;
        // check if any processes need to be moved to cpu queue
        if (incrementor != num_proc) {
            checkProcesses();
        }
    }

    void calcResponse() {
        for (int i = 0; i < complete.size(); i++) {
            complete.at(i).calcRespT();
            complete.at(i).calcTurnT();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    //                       First Come First Serve                         //
    //////////////////////////////////////////////////////////////////////////

    // handling the CPU queue for FCFS
    void FCFS_CPU() {
        while (!CPUQ.empty()) {
            // time upon which process first gets to cpu
            if (CPUQ.front().getFCPUTime() == -1) {
                CPUQ.front().setFCPUTime(clock);
            }
            // if io time is greater than cpu time
            if (CPUQ.front().move_to_IO()) {  // swap to io queue
                IOQ.push_back(CPUQ.front());  // move process to IO queue
                CPUQ.pop_front();             // remove process from CPU queue
            } else { // keep running cpu queue

                CPUQ.front().decCPU();  // decrease cpu burst time

                //CPUQ.front().incResponse();  // inc response time

                // increment wait time for processes in CPUQ, except front
                for (int i = 1; i < CPUQ.size(); i++) {
                    CPUQ.at(i).incWaitT();
                }

                // if both cpu and io times are 0
                // move them to complete queue
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    CPUQ.front().setExitTime(clock+1);
                    complete.push_back(CPUQ.front());
                    CPUQ.pop_front();
                }
            }
            ClockInc();
        }
        if (!IOQ.empty()) {  // if io queue is not empty
            FCFS_IO();
        }
    }

    // handling the IO queue for FCFS
    void FCFS_IO() {
        // while io queue is not empty
        while (!IOQ.empty()) {
            IOQ.front().decIO();  // decrease io burst time

            // increment wait time for processes in IOQ
            for (int i = 0; i < IOQ.size(); i++) {
                IOQ.at(i).incWaitT();
            }

            if (IOQ.front().getIOBurstTime() == 0) {  // if io is done, move back to cpu
                CPUQ.push_back(IOQ.front());
                IOQ.pop_front();
            }
            ClockInc();
        }
        if (!CPUQ.empty()) {
            FCFS_CPU();  // return to CPU
        }
    }

    void FCFS() {
        checkProcesses();  // check processes at clock 0
        FCFS_CPU();        // start by running through cpu queue
        calcResponse();
    }

    //////////////////////////////////////////////////////////////////////////
    //                             Round Robin                              //
    //////////////////////////////////////////////////////////////////////////

    // handling the CPU queue for RR
    void RR_CPU(int q) {
        int counter = 0;
        while (!CPUQ.empty()) {
            // time upon which process first gets to cpu
            if (CPUQ.front().getFCPUTime() == -1) {
                CPUQ.front().setFCPUTime(clock);
            }
            // if io time is greater than cpu time
            if (CPUQ.front().move_to_IO()) {  // swap to io queue
                IOQ.push_back(CPUQ.front());  // move process to IO queue
                CPUQ.pop_front();             // remove process from CPU queue
                counter = 0;  // if process moves to IO queue, reset to 0
            } else {          // keep running cpu queue

                CPUQ.front().decCPU();  // decrease cpu burst time
                // cout << "process "<< CPUQ.front().getPID() << endl;
                //CPUQ.front().incResponse();  // inc response time

                // increment wait time for processes in CPUQ, except front
                for (int i = 1; i < CPUQ.size(); i++) {
                    CPUQ.at(i).incWaitT();
                }
                // if both cpu and io times are 0
                // move them to complete queue
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    CPUQ.front().setExitTime(clock+1);
                    complete.push_back(CPUQ.front());
                    CPUQ.pop_front();
                    counter = 0;  // if process finishes before time quantum,
                                  // reset to 0
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
        if (!IOQ.empty()) {  // if io queue is not empty
            RR_IO(q);
        }
    }

    // handling the IO queue for RR
    void RR_IO(int q) {
        while (!IOQ.empty()) {
            IOQ.front().decIO();  // decrease io burst time

            // increment wait time for processes in IOQ
            for (int i = 0; i < IOQ.size(); i++) {
                IOQ.at(i).incWaitT();
            }

            if (IOQ.front().getIOBurstTime() == 0) {  // if io is done, move back to cpu
                CPUQ.push_back(IOQ.front());
                IOQ.pop_front();
            }
            ClockInc();
        }
        if (!CPUQ.empty()) {
            RR_CPU(q);  // return to CPU
        }
    }

    void RR(int q) {
        checkProcesses();
        RR_CPU(q);
        calcResponse();
    }

    //////////////////////////////////////////////////////////////////////////
    //                       Shortest Process Next                          //
    //////////////////////////////////////////////////////////////////////////

    // handling the CPU queue for SPN
    void SPN_CPU() {
        int shortest;
        deque<int>::iterator it;

        while (!CPUQ.empty()) {
            shortest = 0;
            // time upon which process first gets to cpu
            if (CPUQ.front().getFCPUTime() == -1) {
                CPUQ.front().setFCPUTime(clock);
            }
            // if io time is greater than cpu time
            if (CPUQ.front().move_to_IO()) {  // swap to io queue
                IOQ.push_back(CPUQ.front());  // move process to IO queue
                CPUQ.pop_front();             // remove process from CPU queue
            } else {                          // keep running cpu queue

                CPUQ.front().decCPU();  // decrease cpu burst time

                //CPUQ.front().incResponse();  // inc response time

                // increment wait time for processes in CPUQ, except front
                for (int i = 1; i < CPUQ.size(); i++) {
                    CPUQ.at(i).incWaitT();
                }

                // if both cpu and io times are 0
                // move them to complete queue
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    CPUQ.front().setExitTime(clock+1);
                    complete.push_back(CPUQ.front());
                    CPUQ.pop_front();
                }

                // find process with least amount of time left for cpu burst
                // move process with shortest time to front
                for (int i = 0; i < CPUQ.size(); i++) {
                    if (CPUQ.at(i).getCPUBurstTime() < CPUQ.at(shortest).getCPUBurstTime()) {
                        shortest = i; // location of shortest cpu burst time
                    }
                }

                // move shortest process to the front
                if (!CPUQ.empty()) {
                    if (CPUQ.size() > 1) {
                        CPUQ.push_front(CPUQ.at(shortest));
                        CPUQ.erase(CPUQ.begin()+shortest+1);
                    }
                }
            }
            ClockInc();
        }
        if (!IOQ.empty()) {  // if io queue is not empty
            SPN_IO();
        }
    }

    // handling the IO queue for SPN
    void SPN_IO() {
        // while io queue is not empty
        while (!IOQ.empty()) {
            IOQ.front().decIO();  // decrease io burst time

            // increment wait time for processes in IOQ
            for (int i = 0; i < IOQ.size(); i++) {
                IOQ.at(i).incWaitT();
            }

            if (IOQ.front().getIOBurstTime() == 0) {  // if io is done, move back to cpu
                CPUQ.push_back(IOQ.front());
                IOQ.pop_front();
            }
            ClockInc();
        }
        if (!CPUQ.empty()) {
            SPN_CPU();  // return to CPU
        }
    }

    void SPN() {
        checkProcesses();
        SPN_CPU();
        calcResponse();
    }

};
