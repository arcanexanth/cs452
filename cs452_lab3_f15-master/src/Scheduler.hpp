#ifndef Scheduler_HPP
#define	Scheduler_HPP

#include <vector>
#include <string>
#include "Process.hpp"
#include "Time_Queue.hpp"

class Scheduler {
    
public:
    Scheduler(vector<Process *> &processes);
    Scheduler(const Scheduler& orig);
    string toString() const;
    //bool receivedNewProcess(int clock) const;
    bool hasUnfinishedJobs() const;
    virtual void run();
    double calcAvgWaitTime();
    double calcAvgTurnaroundTime();
    int getTotalProcessesRan();
    virtual ~Scheduler();
protected:
    vector<Time_Queue *> queues;
    vector<Process *> processes;
    int totalProcessesRan;
};

#endif

