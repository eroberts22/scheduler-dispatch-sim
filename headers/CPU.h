#pragma once
// CPU.h
#include <vector>
#include <queue>
#include <algorithm>
#include "process.h"
using namespace std;

class CPU {
private:
    // all processes assigned to CPU
    vector<Process> cpu_processes;

    // algorithm specific queues of processes
    queue<Process> IOQ;        // wait for I/O operation to complete
    queue<Process> CPUQ;       // wait for CPU operation
    vector<Process> complete;   // completed processes

    // clear queue [1]
    void _clear(queue<Process> &q) {
        queue<Process> empty;
        swap( q, empty );
    }

public:
    CPU() {}

    CPU(vector<Process> v) {
        setProcesses(v);
    }

    vector<Process> getProcesses() {
        return cpu_processes;
    }

    void setProcesses(vector<Process> v) {
        // sort the processes by arrival time
        sort(v.begin(), v.end());
        this->cpu_processes = v;
    }

    void addProcess(Process p) {
        cpu_processes.push_back(p);
    }

    void clearAllProcesses() {
        clearQueueProcesses();
        cpu_processes.clear();
    }

    void clearQueueProcesses() {
        _clear(IOQ);
        _clear(CPUQ);
        complete.clear();
    }

    void printAllProcesses() {
        cout << "PID |Arr_t|CPU_t|IO_t |Acc_t\n";
        cout << "----|-----|-----|-----|-----\n";
        for (int i = 0; i < cpu_processes.size(); i++) {
            cout << cpu_processes.at(i).getAllInfo() << endl;
        }
        cout << endl;
    }

    void printCompleteProcesses() {
        cout << "PID |Arr_t|CPU_t|IO_t |Acc_t\n";
        cout << "----|-----|-----|-----|-----\n";
        for (int i = 0; i < complete.size(); i++) {
            cout << complete.at(i).getAllInfo() << endl;
        }
        cout << endl;
    }

    void FCFS_CPU() {
        while(!CPUQ.empty()) {            
            // if io time is greater than cpu time
            if (CPUQ.front().getIOBurstTime() > CPUQ.front().getCPUBurstTime()) {
                IOQ.push(CPUQ.front()); // move process to IO queue
                CPUQ.pop(); // remove process from CPU queue
            } else {
                CPUQ.front().decCPU(); // decrease cpu burst time
                // if both cpu and io times are 0
                if (CPUQ.front().getIOBurstTime() == 0 && CPUQ.front().getCPUBurstTime() == 0) {
                    complete.push_back(CPUQ.front());
                    CPUQ.pop();
                }
            }   
        }
        if(!IOQ.empty()) { // if io queue is not empty
            FCFS_IO();
        }
        
    }

    void FCFS_IO() {
        // while io queue is not empty
        while(!IOQ.empty()) {
            IOQ.front().decIO(); // decrease io burst time
            if (IOQ.front().getIOBurstTime() == 0) { // if io is done, move back to cpu
                CPUQ.push(IOQ.front());
                IOQ.pop();
            }
        }
        if (!CPUQ.empty()) {
            FCFS_CPU(); // return to CPU
        }
    }
    
    void FCFS() {
        // push all processes to CPU queue
        for(int i=0; i<cpu_processes.size(); i++) {
            CPUQ.push(cpu_processes.at(i));
        }
        FCFS_CPU(); // start by running through cpu queue
    }

    void RR(int q) {}

    void SPN() {}

};
