#include <iostream>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include <boost/foreach.hpp>
#include "Process.hpp"
#include "WHS.hpp"
#include "RBTree.hpp"

//#define DEBUG

using namespace std;



WHS::WHS(vector<Process *> &processes, vector<Process *> &processesByPID, int quantum, int ageLimit) 
			: Scheduler(processes),
            quantum(quantum),
            ageLimit(ageLimit),
            processesByPID(processesByPID)
             {

    // Make a tree to hold lists of processes at every priority
    tree = rbt.rbtree_create();

    // Make a temp vector to hold all the lists
    vector<list<Process*>* > tempVector;

    // Populate the vector with new lists (deleted later)
    for (int i = 0; i <= 99; i++) {
        tempVector.push_back(new list<Process*>());
    }

    // Assign it to the member variable
    listVector = tempVector;

    // Populate the terminated processes vector
    // Note: the index will be 1 less than the PID
    for (int i = 0; i < (int)processes.size(); i++) {
        terminatedProcesses.push_back(false);
    }

    // Resets all bits to 0
    listsInTree.reset();
}

WHS::~WHS(){
}

void WHS::run() {

    int clock = 0;
    while (this->hasNonTerminatedJobs()/* && clock < 3900*/) {
        
#ifdef DEBUG
        cout << endl << "*****Beginning of clock tick: " << clock << "******" << endl;
#endif
        // For stress testing
        if (clock % 10000 == 0) {
            cout << "clock: " << clock << endl;
        }

        receiveNewJobs(clock);

        // IF TREE IS EMPTY, JUST ADVANCE CLOCK TICK
        if (tree->root != NULL) {

#ifdef DEBUG
            cout << "Tree of Priority Lists: " << endl;
            printTree();
#endif

            // Choose the highest priority process that is ready to run.
            list<Process*> *maxList = getMaxPriorityList();


            // Run the first process in the list and pop it off the list.
            Process* p = maxList->front();
            
#ifdef DEBUG
            cout << "Highest priority list (" << p->getPriority() << "):" << endl;
            // for (list<Process*>::iterator it = maxList->begin(); it != maxList->end(); ++it) {
            //     cout << "   " << (*it)->toString() << endl;
            // }
#endif

            maxList->pop_front();

#ifdef DEBUG
            cout << "**Loaded into CPU: process: " << p->toString() << endl;
#endif

            // Remove the list from the tree if it's now empty.
            if (maxList->empty()) {
                int priority = p->getPriority();
                deleteFromTree(priority);
            }

            int runTimer;
            int io = p->getIO();
            int timeRemaining = p->getTimeRemaining();
            bool killAfterTQ = false;
            bool doesIO = false;
            if (timeRemaining <= quantum) { // Process terminates
                runTimer = timeRemaining;
                killAfterTQ = true;
#ifdef DEBUG
            cout << "Process " << p->getPID() << " will finish before the time quantum is up. Running for remaining burst:  " << runTimer << " clock ticks." << endl;
#endif
            } else if (io > 0) { // Process does I/O at 2nd-to-last clock tick of TQ
                runTimer = quantum - 1;
                p->resetIOTimer();
                doesIO = true;
#ifdef DEBUG
            cout << "Process " << p->getPID() << " will do I/O. Running for (time quantum - 1):  " << runTimer << " clock ticks." << endl;
#endif
            } else { // Process does not do I/O and does not finish
                runTimer = quantum;
#ifdef DEBUG
            cout << "Process " << p->getPID() << " is CPU bound (no I/O). Running for full time quantum of " << runTimer << " clock ticks." << endl;
#endif
            }

            // Save total time in CPU
            int timeInCPU = runTimer;

            // The difference between this clock tick and the end of
            // its last run session is its added to its wait time.
            // p->addTimeWaiting(clock - p->getExitCPUTick());

            while (runTimer > 0) {
#ifdef DEBUG
                cout << "Process in CPU: " << p->toString() << endl;
                cout << "Remaining time in CPU: " << runTimer << endl;
#endif
                p->decrementTimeRemaining();

                // Age processes
                ageAllProcessesInTree();

                // Adjust I/O wait queue
                adjustIOQueue();

                runTimer--;
                clock++;
                if (runTimer > 0) {
#ifdef DEBUG
                    cout << endl << "**Beginning of clock tick: " << clock << endl;
#endif        
                    // For stress testing
                    if (clock % 10000 == 0) {
                        cout << "2clock: " << clock << endl;
                    }

                    receiveNewJobs(clock);
                }
            }

            if (killAfterTQ) { // Process finishes
                p->setFinishTime(clock);
                p->setState(Process::TERMINATED);

                int index = find(processes.begin(), processes.end(), p) - processes.begin();
                terminatedProcesses.at(index) = true;
#ifdef DEBUG
                cout << "Process " << p->getPID() << " completed. Terminated. " << endl;
#endif
            } else if (doesIO) { // Process does I/O; send to wait queue
                waitQueue.push_back(p);
#ifdef DEBUG
                cout << "Process " << p->getPID() << " doing I/O. Sent to wait queue for " << p->getIO() << " ticks. " << endl;
#endif
            } else { // Process did not finish and does not do I/O; demote process
#ifdef DEBUG
                cout << "Process " << p->getPID() << " did not finish (remaining burst: " << p->getTimeRemaining() << "). Demoting." << endl;
#endif
                int priorityBefore = p->getPriority();

                // Decrement priority by amount of clock ticks spent in CPU
                bool fullDecrement = p->decrementPriority(timeInCPU);
#ifdef DEBUG
                if (fullDecrement) {
                    cout << "Process " << p->getPID() << " priority changed from " << priorityBefore << " to " << p->getPriority() << endl;
                } else if (priorityBefore != p->getPriority()) {
                    cout << "Process " << p->getPID() << " priority changed from " << priorityBefore << " to " << p->getPriority() << " (reached minimum)" << endl;
                } else {
                    cout << "Process " << p->getPID() << " priority not changed (still " << p->getPriority() << ", reached minimum)" << endl;                    
                }
#endif

                // Move the process to its appropriate priority queue
                // Since the priority is already changed, the process will
                // be added to the end of the correct queue.
                addProcessToPriorityList(p);

                // Every process that is demoted resets its age timer
                p->setAge(0);

            }

            // For wait time calculation, set exit CPU clock-1
            // because clock was advanced once at end of loop
#ifdef DEBUG
            cout << "Setting exitCPUTick to " << clock-1 << endl;
#endif
            // p->setExitCPUTick(clock-1);
        } else {
#ifdef DEBUG
            cout << "Priority list tree is empty." << endl;
#endif
            // Adjust I/O wait queue
            adjustIOQueue();
            clock++;
        }
        // Advance clock tick when no jobs are in the tree.
    } // end while (this->hasNonTerminatedJobs())









    // Delete all the priority lists
    for (int i = 0; i < (int)listVector.size(); i++) {
        delete listVector[i];
    }

    delete tree;

    // In WHS, all processes are scheduled.
    this->totalProcessesRan = processes.size();
}

// Ages every process in every list in the tree of priority lists.
// Also adds 1 to wait time of each process.
void WHS::ageAllProcessesInTree() {
    for (int i = 0; i <= 99; i++) {
        if (listsInTree.test(i)) { // if true, list is in tree
            list<Process*> *curList = listVector.at(i);
#ifdef DEBUG
            // cout << "Aging all in list " << i << ":" << endl;
            // for (list<Process*>::iterator it = curList->begin(); it != curList->end(); ++it) {
            //     cout << "   " << (*it)->toString() << endl;
            // }
#endif
            for (list<Process*>::iterator it = curList->begin(); it != curList->end(); ++it) {
                Process* p = *it;
                // Add 1 to each process's age
                p->addAge(1);
                // Add 1 to wait time
                p->addTimeWaiting(1);
                // Promote the process (by 10 priority) if its aging timer expires
                if (p->getAge() > ageLimit) {
#ifdef DEBUG
                    cout << "Process " << p->getPID() << " aging timer expired." << endl;
#endif
                    int priorityBefore = p->getPriority();
                    p->incrementPriority(10);
                    // If it changed, move it
                    if (priorityBefore < p->getPriority()) {
                        // Probably not the best way to do this...
                        list<Process*>::iterator temp = it;
                        ++it;
                        removeProcessFromPriorityList(temp, priorityBefore);
                        addProcessToPriorityList(p);
                    } else {
#ifdef DEBUG
                        cout << "Priority not changed due to aging, reached maximum. (still" << p->getPriority() << ")." << endl;
#endif
                    }
                    p->setAge(0);
#ifdef DEBUG
                    cout << "   Reset age of process " << p->getPID() << " to 0." << endl;
#endif
                }
            }
        }
    }
}

bool WHS::hasNonTerminatedJobs() {
    bool found = false;
    int i = 0;
    while (!found && (i < (int)terminatedProcesses.size())) {
        if (!terminatedProcesses.at(i)) {
            found = true;
        }
        i++;
    }
    return found;
}

void WHS::adjustIOQueue() {
    // Subtract one from each process's I/O timer
#ifdef DEBUG
    cout << "Checking I/O List:" << endl;
#endif
    BOOST_FOREACH(Process *p, waitQueue) {
        p->decrementIOTimer();
#ifdef DEBUG
        cout << "   Process " << p->getPID() << " IOTimer: " << p->getIOTimer() << endl;
#endif
    }

    bool stillZero = true;
    deque<Process*>::iterator it = waitQueue.begin();
    Process* p;
    while (/*stillZero && */(it != waitQueue.end())) {
        p = *it;
        if (p->getIOTimer() == 0) {
            // Boost priority based on I/O, add to appropriate priority list, and remove process from wait queue.
#ifdef DEBUG
            cout << "IOTimer expired for process " << p->getPID() << ". Adding " << p->getIO() << " to priority." << endl;
#endif
            p->incrementPriority(p->getIO());

            addProcessToPriorityList(p);

            // deque::erase returns the iterator following the last removed element,
            // so no need to increment the iterator.
            it = waitQueue.erase(it);
        } else {
            ++it;
            stillZero = false;
        }
    }
}

list<Process*>* WHS::getMaxPriorityList() {
    return (list<Process*>*)rbt.maximum_node(tree->root)->value;
}

void WHS::insertIntoTree(int priority, list<Process*> *list) {
    rbt.rbtree_insert(tree, (void*)priority, (void*)list, RBTree::compare_int);
    listsInTree.set(priority); // Set bit to 1
}

int WHS::deleteFromTree(int priority) {
    int retVal = rbt.rbtree_delete(tree, (void*)priority, RBTree::compare_int);
    listsInTree.reset(priority); // Reset bit to 0
    return retVal;
}

void WHS::addProcessToPriorityList(Process* p) {
    int priority = p->getPriority();
    list<Process*> *priorityList = listVector.at(priority);
    // If the list is empty, add the list to the tree
    if (priorityList->empty()) {
        insertIntoTree(priority, priorityList);
#ifdef DEBUG
        cout << "List " << priority << " was empty. Added to tree:" << endl;
        printTree();
#endif
    } else {
#ifdef DEBUG
        cout << "List " << priority << " was not empty. Not added to tree." << endl;
        printTree();
#endif

    }

    priorityList->push_back(p);
#ifdef DEBUG
    cout << "Added " << p->getPID() << " to list " << priority << endl;
    // cout << "List " << priority << ": " << endl;

    // for (list<Process*>::iterator it = priorityList->begin(); it != priorityList->end(); ++it) {
    //     cout << "   " << (*it)->toString() << endl;
    // }
#endif
}

void WHS::removeProcessFromPriorityList(list<Process*>::iterator it, int priority) {
    list<Process*> *priorityList = listVector.at(priority);
    Process* curP;
    curP = *it;
    priorityList->erase(it);
#ifdef DEBUG
    cout << "Process " << curP->getPID() << " removed from list " << priority << endl;
#endif
    if (priorityList->empty()) {
        deleteFromTree(priority);
#ifdef DEBUG
        cout << "List " << priority << " now empty. Deleted from tree: " << endl;
        printTree();
#endif
    }

}

bool WHS::receiveNewJobs(int clock) {
    bool foundNewProcess = false;
    int i = 0;
    bool foundLaterProcess = false;
    Process* p;
    // Since processes are in order of their arrival, stop loop
    // when a later process is found.
    while (i < (int)processes.size() && !foundLaterProcess) {
        p = processes.at(i);
        if ((p->getState() == Process::NEW) && (p->getArrivalTime() <= clock)) { // new, earlier process
            // For debugging, lets us see the time waiting earlier
            p->addTimeWaiting(clock - p->getExitCPUTick());
            
            // Reset the exitCPUTick to the current clock,
            // otherwise the time since its arrival will be counted twice.
            p->setExitCPUTick(clock);

            p->setState(Process::READY_TO_RUN);
#ifdef DEBUG
            cout << "New arrival at clock " << clock << ": adding process " << p->getPID() << " to list " << p->getPriority() << endl;
#endif
            // Add the process to its appropriate priority list
            addProcessToPriorityList(p);
            foundNewProcess = true;
        } else if (p->getArrivalTime() > clock) { // later process
            foundLaterProcess = true;
        }
        i++;
    }
    return foundNewProcess;
}

void WHS::printTree() {
    rbt.print_tree(tree);
}
