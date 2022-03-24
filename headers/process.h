#pragma once
// Process.h
#include <string>
using namespace std;

class Process {
private:
    int PID;
    int arrival_t;      // starting time
    int cpu_burst_t;    // amount of time in cpu queue
    int io_burst_t;     // amount of time in io queue
    int accumulated_t;  // total amount of time
    int wait_t;         // amount of time waiting 
    int response_t;     // how long response time is
public:
    Process () {}
    ~Process() {}

    Process (int p, int a, int c, int i) {
        setPID(p);
        setArrivalTime(a);
        setCPUBurstTime(c);
        setIOBurstTime(i);
        setWaitTime(0);
        setResponseTime(0);
        setAccTime(0);
    }

    // true if process io time is greater than cpu time
    // false if process cpu time is greater than io time
    // used to switch processes between the io and cpu queues
    bool move_to_IO() {
        if (getIOBurstTime() > getCPUBurstTime()) { return true;}
        else {return false;}
    }

    string getAllInfo() {
        return to_string(getPID()) 
        + " |   " + to_string(getArrivalTime()) 
        + " |   " + to_string(getCPUBurstTime()) 
        + " |   " + to_string(getIOBurstTime()) 
        + " |   " + to_string(getAccTime());
        
    }

    // get and set //////////////////////
    int getPID() {
        return PID;
    }

    int getArrivalTime() {
        return arrival_t;
    }

    int getCPUBurstTime() {
        return cpu_burst_t;
    }

    int getIOBurstTime() {
        return io_burst_t;
    }

    // same as turnaround?
    int getAccTime() {
        return accumulated_t;
    }

    int getWaitTime() {
        return wait_t;
    }

    int getResponseTime() {
        return response_t;
    }

    void setPID(int n) {
        this->PID = n;
    }

    void setArrivalTime(int n) {
        this->arrival_t = n;
    }

    void setCPUBurstTime(int n) {
        this->cpu_burst_t = n;
    }

    void setIOBurstTime(int n) {
        this->io_burst_t = n;
    }
        
    void setAccTime(int n) {
        this->accumulated_t = n;
    }

    void setWaitTime(int n) {
        this->wait_t = n;
    }

    void setResponseTime(int n) {
        this->response_t = n;
    }

    // decrement remaining I/O burst time
    void decIO() {
        this->io_burst_t--;
    }

    // decrement remaining CPU burst time
    void decCPU() {
        this->cpu_burst_t--;
    }

    // increment wait time


    // increment response time
};
