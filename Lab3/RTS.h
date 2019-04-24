#ifndef RTS_H
#define RTS_H

#include <cstdlib>
#include <vector>
#include <iostream>
#include <iterator>
#include <cstring>
#include <map>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <algorithm>
#include "process.h"
#include <queue>
#include <functional>

using namespace std;

struct Comparator
{
    bool operator()(const process& a, const process& b) const{
        if(a.ddLine == b.ddLine){
            return a.pid > b.pid;
        }else{
            return a.ddLine > b.ddLine;
        }
    }
};

class RTS{
    private:
        process_map pMap;
        uint32_t currTick = 0;
        bool hardRTS = false;
        uint32_t turnAround = 0;
        uint32_t processed = 0;
        uint32_t failed = 0;
        priority_queue<process, processes, Comparator> pQueue;
        priority_queue<process, processes, Comparator> pCopy;
        bool increaseCurrTick = false;

        void doTheNeedful();
        void runProcess();
        void queueProcesses();
        void sortByDeadLine();
        void sortByPID();
        void sortByDeadline();
        static bool compareByDeadline(const process &a, const process &b);
        void error(const char* msg, process p);
        void print_stats();
        template<typename A> void print_queue(A& queue);
    public:
        RTS(process_map pMap);
};
#endif
