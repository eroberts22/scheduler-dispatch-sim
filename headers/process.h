#pragma once
// Process.h
#include <string>
using namespace std;

class Process {
   private:
    int PID;
    int arrival_t;     // starting time
    int cpu_burst_t;   // amount of time in cpu queue, will be decremented
    int io_burst_t;    // amount of time in io queue, will be decremented
    int turnaround_t;  // time in system - includes bursts and wait time
    int wait_t;        // amount of time waiting
    int response_t;    // how long response time is

    int o_cpu_burst_t;  // overall cpu burst
    int o_io_burst_t;   // overall io burst
   public:
    Process() {}
    ~Process() {}

    Process(int p, int a, int c, int i) {
        setPID(p);
        setArrivalTime(a);
        setCPUBurstTime(c);
        setIOBurstTime(i);
        setWaitTime(0);
        setResponseTime(0);
        setTurnTime(0);
        setOCPUBurstTime(c);
        setOIOBurstTime(i);
    }

    // true if process io time is greater than cpu time
    // false if process cpu time is greater than io time
    // used to switch processes between the io and cpu queues
    bool move_to_IO() {
        if (getIOBurstTime() > getCPUBurstTime()) {
            return true;
        } else {
            return false;
        }
    }

    string getAllInfo() {
        return to_string(getPID()) 
        + " |   " + to_string(getArrivalTime()) 
        + " |   " + to_string(getOCPUBurstTime()) 
        + " |   " + to_string(getOIOBurstTime()) 
        + " |    " + to_string(getWaitTime()) 
        + " |    " + to_string(getResponseTime()) 
        + " |    " + to_string(getTurnTime());
    }

    // [2] operator used to sort processes by arrival time
    bool operator<(const Process& p) const { return (arrival_t < p.arrival_t); }

    // get and set //////////////////////
    int getPID() { return PID; }

    int getArrivalTime() { return arrival_t; }

    int getCPUBurstTime() { return cpu_burst_t; }

    int getIOBurstTime() { return io_burst_t; }

    int getTurnTime() { return turnaround_t; }

    int getWaitTime() { return wait_t; }

    int getResponseTime() { return response_t; }

    int getOCPUBurstTime() { return o_cpu_burst_t; }

    int getOIOBurstTime() { return o_io_burst_t; }

    void setPID(int n) { this->PID = n; }

    void setArrivalTime(int n) { this->arrival_t = n; }

    void setCPUBurstTime(int n) { this->cpu_burst_t = n; }

    void setIOBurstTime(int n) { this->io_burst_t = n; }

    void setTurnTime(int n) { this->turnaround_t = n; }

    void setWaitTime(int n) { this->wait_t = n; }

    void setResponseTime(int n) { this->response_t = n; }

    void setOCPUBurstTime(int n) { this->o_cpu_burst_t = n; }

    void setOIOBurstTime(int n) { this->o_io_burst_t = n; }

    // decrement remaining I/O burst time
    void decIO() {
        if (io_burst_t > 0) {
            this->io_burst_t--;
        }
    }

    // decrement remaining CPU burst time
    void decCPU() {
        if (cpu_burst_t > 0) {
            this->cpu_burst_t--;
        }
    }

    void incTurnT() { this->turnaround_t++; }

    // increment wait time
    void incWaitT() {
        this->wait_t++;
        this->turnaround_t++;
    }

    // increment response time
    void incResponse() {
        this->response_t++;
        this->turnaround_t++;
    }
};
