#ifndef WHS_HPP
#define	WHS_HPP

#include <list>
#include <bitset>
#include "Scheduler.hpp"
#include "RBTree.hpp"

class WHS : public Scheduler {

public:
	WHS(vector<Process *> &processes, vector<Process *> &processesByPID, int quantum, int ageLimit);
	virtual ~WHS();
    void run();
    void ageAllProcessesInTree();
    bool hasNonTerminatedJobs();
    void adjustIOQueue();
    list<Process*>* getMaxPriorityList();
    void insertIntoTree(int priority, list<Process*> *list);
    int deleteFromTree(int priority);
    void addProcessToPriorityList(Process* p);
    void removeProcessFromPriorityList(list<Process*>::iterator it, int priority);
    bool receiveNewJobs(int clock);
    void printTree();
private:
	int quantum;
	int ageLimit;
    RBTree rbt;
    rbtree tree;
    deque<Process*> waitQueue;
    vector<list<Process*>* > listVector;
    // Empty constructor initializes all elements to 0.
    bitset<100> listsInTree;
    // terminated = true
    vector<bool> terminatedProcesses;
    vector<Process *> processesByPID;


};

#endif	/* WHS_HPP */