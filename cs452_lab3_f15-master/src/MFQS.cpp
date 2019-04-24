#include <iostream>
#include <algorithm>
#include <deque>
#include <boost/foreach.hpp>
#include "MFQS.hpp"
#include "Scheduler.hpp"
#include "Time_Queue.hpp"

//#define DEBUG

using namespace std;

MFQS::MFQS(vector<Process *> &processes, int quantum, int numberOfQueues, int ageLimit)
        : Scheduler(processes), 
        ageLimit(ageLimit), quantum(quantum), numberOfQueues(numberOfQueues) {
    Time_Queue *readyToRunQ = new Time_Queue(quantum);

    // Add the ready to run queue and then add the other RR,
    // lastly add the FCFS queue to the end.
    this->queues.push_back(readyToRunQ);
    for (int i = 0; i < numberOfQueues - 2; i++) {
        quantum *= 2;
        Time_Queue *currentQ = new Time_Queue(quantum);
        this->queues.push_back(currentQ);
    }
    Time_Queue *fcfs = new Time_Queue();
    this->queues.push_back(fcfs);

    // Add jobs with arrival 0 to first queue
    receiveNewJobs(0);
}

MFQS::~MFQS(){
}

void MFQS::run() {
    int clock = 0;
    while (this->hasUnfinishedJobs()) {
        int i = 0;
        int lastQueue = numberOfQueues - 1;
        bool newProcessesAdded;
        int jobAged;

        // !!TEMP!!
        int iteration;

        while (i <= lastQueue) {
            Time_Queue *queue = this->queues.at(i);
            bool queueIsEmpty = queue->empty();
#ifdef DEBUG
            if (!queueIsEmpty) {
                cout << "**" << endl;
                cout << "Clock " << clock << ": Currently running in queue " << i << ": " << queue->toString();
                cout << "**" << endl << endl;
            }
#endif
            newProcessesAdded = receiveNewJobs(clock);
            jobAged = -1;
            while (!queueIsEmpty && !newProcessesAdded && (jobAged < 0)) {

                jobAged = -1;
                Process *p = queue->pop();

                int timeRan;
                if (i == lastQueue) { // Last queue is FCFS
                    timeRan = p->getTimeRemaining();

                    // The difference between this clock tick and the end of
                    // its last run session is its added to its wait time.
                    p->addTimeWaiting(clock - p->getExitCPUTick());

#ifdef DEBUG
                    cout << "clock before: " << clock << endl;
#endif

                    clock += timeRan;

                    // !!TEMP!!
                    iteration++;

                    jobAged = age(i, p, timeRan);

                    // Terminate the process
                    p->setTimeRemaining(0);
                    p->setFinishTime(clock);
                    p->setState(Process::TERMINATED);

#ifdef DEBUG
                    cout << "****FCFS****" << endl;
                    cout << "Process " << p->getPID() << " ran for " << timeRan << " in queue " << i << " and terminated." << endl;
                    cout << "clock after: " << clock << endl << endl;
#endif
                } else { // All queues before last are TQ
                    p->setState(Process::RUNNING);

                    if (p->getTimeRemaining() <= queue->getQuantum()) { // Process will finish in this TQ
                        timeRan = p->getTimeRemaining();
                        p->addTimeWaiting(clock - p->getExitCPUTick());
#ifdef DEBUG
                        cout << "clock before: " << clock << endl;
#endif

                        clock += timeRan;

#ifdef DEBUG
                        cout << "Process " << p->getPID() << " ran for " << timeRan << " in queue " << i << " and terminated." << endl;
                        cout << "clock after: " << clock << endl << endl;
#endif
                        // !!TEMP!!
                        iteration++;

                        jobAged = age(i, p, timeRan);

                        // Terminate the process
                        p->setTimeRemaining(0);
                        p->setFinishTime(clock);
                        p->setState(Process::TERMINATED);

                    } else { // Process will not finish in this TQ
                        timeRan = queue->getQuantum();
                        p->addTimeWaiting(clock - p->getExitCPUTick());
#ifdef DEBUG
                        cout << "clock before: " << clock << endl;
#endif
                        clock += timeRan;
                        p->setTimeRemaining(p->getTimeRemaining() - timeRan);

#ifdef DEBUG
                        cout << "Proccess " << p->getPID() << " ran for " << timeRan << " in queue " << i
                            << ", time_remaining: " << p->getTimeRemaining()
                            << ", time_waiting: " << p->getTimeWaiting() << endl;
                        cout << "clock: " << clock << endl << endl;
#endif

                        // !!TEMP!!
                        iteration++;

                        jobAged = age(i, p, timeRan);

                        // Set the stop clock tick to be the current clock. Next time the process runs,
                        // the difference between the current clock tick and then will be added to its wait time.
                        p->setExitCPUTick(clock);
                        p->setState(Process::READY_TO_RUN);

                        // Demote the process
                        this->queues.at(i + 1)->push(p);

#ifdef DEBUG
                        cout << "Proccess " << p->getPID() << " added to Queue " << i+1 << ": " << queues.at(i + 1)->toString() << endl;
#endif

                    }
                }

                queueIsEmpty = queue->empty();
                newProcessesAdded = receiveNewJobs(clock);
                // !!TEMP!!
                if ((iteration % 1000) == 0)
                    cout << "clock: " << clock << endl;

            } // end while(!queueIsEmpty && !newProcessesAdded && (jobAged < 0))

            // Change i based on why the loop exited
            if (newProcessesAdded) {
                i = 0;
            } else if (queueIsEmpty) { // only check if the queue is empty if there were no new processes
                i++;
            }

        } // end while(i <= lastQueue)

        
#ifdef DEBUG
        //cout << "All queues ran. Clock: " << clock << endl << endl;
#endif

        // Increase clock tick so later arrivals get to run 
        clock++;

    } // end while(hasUnfinishedJobs()) 


    for (int i = 0; i < numberOfQueues; i++) {
        delete this->queues.at(i);
    }

    // In MFQS, all processes are scheduled.
    this->totalProcessesRan = processes.size();
}

// receiveNewJobs changes the state of all processes that
// have arrived <= clock to READY_TO_RUN, returns whether there
// were any such processes, and adds them to the first queue
bool MFQS::receiveNewJobs(int clock) {
    bool foundNewProcess = false;
    if (clock >= 0) {
        BOOST_FOREACH(Process *p, processes) {
            if ((p->getState() == Process::NEW) &&
                        (p->getArrivalTime() <= clock)) {
                // The process has been waiting since its arrival
                // (exitCPUTick is set to arrival_time upon creation)
                p->addTimeWaiting(clock - p->getExitCPUTick());

                // Reset the exitCPUTick to the current clock,
                // otherwise the time since its arrival will be counted twice.
                p->setExitCPUTick(clock);
                p->setState(Process::READY_TO_RUN);
                // Add the process to the first queue
                this->queues.at(0)->push(p);
                foundNewProcess = true;
#ifdef DEBUG
                cout << "New arrival: adding process " << p->getPID() << " to first queue" << endl;
#endif
            }
        }
    }
    return foundNewProcess;
}

int MFQS::age(int curQ, Process* p, int timeRan) {
    int jobAged = -1;
    int startQ;
    // Only add age to queues >= 2 (3rd or lower queues)
    if (curQ < 2) {
        startQ = 2;
    } else {
        startQ = curQ;
    }

    for (int i = startQ; i <= numberOfQueues - 1; i++) {
        Time_Queue* q = queues.at(i);
#ifdef DEBUG
        //cout << "q[" << i << "]->size(): " << q->size() << endl;
#endif
        // Use an iterator to traverse the queue and add age to all processes.
        deque<Process *> deq = q->getQueue();
        std::deque<Process *>::iterator it = deq.begin();
        Process* p;
        while (it != deq.end()) {
            p = *it;
            p->addAge(timeRan);
#ifdef DEBUG
            cout << "Added to age of process " << p->getPID() << ", now: " << p->getAge() << endl;
#endif
            if (p->getAge() > ageLimit) {
                if (jobAged == -1) {
                    jobAged = i-1;
                }
                p->setAge(0);
                q->pop();
                queues.at(i-1)->push(p);
#ifdef DEBUG
                cout << "PROCESS " << p->getPID() << " AGED TO QUEUE " << i-1 << ": " << queues.at(i-1)->toString();
#endif
            }
            ++it;
        }
    }

    return jobAged;
}
