#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include "process.h"
class MFQS {
    private:
        int currQueue = 0;
        int tQuantum = 0;
        int ageLimit = 0;
        uint32_t totalProcesses = 0;
        uint32_t turnAround = 0;
        std::queue<process> procQueue;
        std::queue<process> fcfsQueue;
	    std::vector<std::queue<process>> queues;
        process_map pMap;


        void setCurrQueue();
	    bool queuesEmpty();
        bool quantumCheck(process p);
        void createQueues(int numQ);
        void do_the_fcfs();
        void addProcessesToFCFSQueue(uint32_t currTick);
        void do_the_needful();
        process runQueuesProcesses(process p, uint32_t currTick);
        void ageProcesses();
        void addProcessesToQueueZero(uint32_t currTick);
        process fcfsOnlyRunProcess(process p, uint32_t currTick);
        void print_stats();

    public:
	    MFQS(process_map pMap, int quantum, int numQ, int ageLimit);
	    void run();
	    

};
