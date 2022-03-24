#pragma once
// CPU.h
#include <vector>
#include <queue>
#include "process.h"
using namespace std;

class CPU {
private:
    // all processes assigned to CPU
    vector<Process> cpu_processes;

    // algorithm specific queues of processes
    queue<Process> IOQ;        // wait for I/O operation to complete
    queue<Process> CPUQ;       // wait for CPU operation
    queue<Process> complete;   // completed processes

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
        _clear(complete);
    }

    void printAllProcesses() {
        cout << "PID |Arr_t|CPU_t|IO_t |Acc_t\n";
        cout << "----|-----|-----|-----|-----\n";
        for (int i = 0; i < cpu_processes.size(); i++) {
            cout << cpu_processes.at(i).getAllInfo() << endl;
        }
    }

    void FCFS() {}

    void RR(int q) {}

    void SPN() {}

};
