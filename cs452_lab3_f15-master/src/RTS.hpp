#ifndef RTS_HPP
#define	RTS_HPP

#include "Scheduler.hpp"

class RTS : public Scheduler {
    
public:
    RTS(vector<Process *> &processes);
    void run();
    virtual ~RTS();
	//void loadArrivals(int clock, int jobsLoaded, deque<Process*> rtsQueue, char softOrHard, bool hardRealTimeStop);
private:

};

#endif	/* RTS_HPP */

