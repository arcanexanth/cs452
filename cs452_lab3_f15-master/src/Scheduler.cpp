#include <boost/foreach.hpp>
#include <sstream>
#include "Scheduler.hpp"
#include "Time_Queue.hpp"
#include <iostream>

//#define DEBUG

Scheduler::Scheduler(vector<Process *> &processes) 
    : processes(processes){
}

Scheduler::Scheduler(const Scheduler& orig) {
    this->queues = orig.queues;
}

Scheduler::~Scheduler() {
}

// Checks if there are processes on the list
// that are not terminated.
bool Scheduler::hasUnfinishedJobs() const {
    bool hasUnfinishedJobs = false;
    int i = 0;

    while (!hasUnfinishedJobs && i < (int)this->processes.size()) {
        Process* p = this->processes.at(i);
        if (p->getState() != Process::TERMINATED) {
            hasUnfinishedJobs = true;
        }
        i++;
    }

    return hasUnfinishedJobs;
}

string Scheduler::toString() const {
    stringstream s;
    BOOST_FOREACH(Time_Queue *currentQ, this->queues) {
        s << currentQ->toString() << endl;
    }
    return s.str();
}

void Scheduler::run(){}

double Scheduler::calcAvgWaitTime() {
    double totalWait;
    BOOST_FOREACH(Process *p, processes) {
#ifdef DEBUG
        cout << "Process " << p->getPID() << " wait time: " << p->getTimeWaiting() << endl;
#endif
        totalWait += p->getTimeWaiting();
    }
#ifdef DEBUG
    cout << "Number of processes: " << processes.size() << endl << endl;
#endif
    double avg = totalWait / totalProcessesRan;
    return avg;
}
    
double Scheduler::calcAvgTurnaroundTime() {
    double totalTurnaround;
    int curTurnaround;
    BOOST_FOREACH(Process *p, processes) {
        curTurnaround = p->getFinishTime() - p->getArrivalTime();
#ifdef DEBUG
        cout << "Process " << p->getPID() << " turnaround: " << curTurnaround << endl;
#endif
        totalTurnaround += curTurnaround;
    }
#ifdef DEBUG
    cout << "Number of processes: " << processes.size() << endl << endl;
#endif
    double avg = totalTurnaround / totalProcessesRan;
    return avg;
}

int Scheduler::getTotalProcessesRan() {
    return totalProcessesRan;
}
