#ifndef Time_QUEUE_HPP
#define	Time_QUEUE_HPP

#include <queue>
#include <string>
#include <deque>
#include "Process.hpp"

using namespace std;

class Time_Queue {
    
public:
    Time_Queue(int quantum);
    Time_Queue();
    void push(Process *p);
    Process* pop();
    Process front();
    string toString() const;
    bool empty() const;
    bool hasReadyProcess() const;
    int size() const;
    int getQuantum() const;
    deque<Process *> getQueue() const;
    virtual ~Time_Queue();
    
private:
    int quantum;
    bool fcfs;
    deque<Process *> queue;
};

#endif	/* MFQ_QUEUE_HPP */

