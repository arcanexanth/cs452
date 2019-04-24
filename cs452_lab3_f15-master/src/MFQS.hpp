#ifndef MFQS_HPP
#define	MFQS_HPP

#include "Scheduler.hpp"


class MFQS : public Scheduler {
    
public:
    MFQS(vector<Process *> &processes, int quantum, int numberOfQueues, int ageLimit);
    virtual ~MFQS();
    void run();
    bool receiveNewJobs(int clock);
    int age(int curQ, Process* p, int timeRan);
private:
	int ageLimit;
	int quantum;
	int numberOfQueues;

};

#endif	/* MFQS_HPP */

