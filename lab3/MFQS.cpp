#include <iostream>
#include <vector>
#include <queue>
#include "MFQS.h"

//#define DEBUG

using namespace std;

MFQS::MFQS(process_map pMap, int quantum, int numQ, int ageLimit) {
    this->pMap = pMap;
    this->tQuantum = quantum;
    this->ageLimit = ageLimit;
	cout << "MFQS stuff" << endl;
	bool onlyFCFS = false;
    if(numQ > 1){	
        createQueues(numQ);	
        do_the_needful();
    }else{
        onlyFCFS = true;
        do_the_fcfs();
    }
	uint64_t currArrTime = 0;

/*if(!onlyFCFS){
	int currQ = 0;
	uint64_t numProcesses = 0;
	uint64_t totalTurnaroundTime = 0;
	bool toNextProc = true;
	process p;
	vector<process> procs;
	do {
	//insert all processes with identical arrival time at once
		procs = pMap[currArrTime];
		for(int i = 0; i < procs.size(); i++) {
			numProcesses++;
			queues[0].push(procs[i]);
			#ifdef DEBUG
			cout << "INSERTING " << procs[i];
			#endif
			if(toNextProc)
			currQ = 0;
		}
		if(toNextProc) {
			p = queues[currQ].front();
			toNextProc = false;
		}
		#ifdef DEBUG
		cout << "CURR ARR TIME: " << currArrTime << " currQ: " << currQ <<  endl;
		#endif
		currArrTime++;
		//Update process time
			if(!queues[currQ].empty()) {
				#ifdef DEBUG
				cout << "currQ: " << currQ << endl;
				#endif
				//update current process's pTime
				p.pTime++;
				#ifdef DEBUG
				cout << p;
				#endif
				if(p.pTime == p.bst) {
				//if process time == burst, remove process from queue
					#ifdef DEBUG
					cout << "REMOVING FROM QUEUE " << queues[currQ].front();
					#endif
					totalTurnaroundTime += (currArrTime - queues[currQ].front().arr); 
					queues[currQ].pop();
					toNextProc = true;
					//fcfs proc no longer running
					//if new things have been added to top queue while process was running, set currQ to top queue
					if(!queues[0].empty()) {
						currQ = 0;
					}
				} else if (currQ < numQ-1 && p.ticks == quantum * (currQ + 1)) {
					//if not last queue	remove process from curr queue and move it to next queue
					#ifdef DEBUG
					cout << "MOVING TO NEXT QUEUE" << endl;
					cout << currQ << endl;
					cout << "quantum: " << quantum * (currQ + 1) << endl;
					#endif
					toNextProc = true;	
					p.ticks = 0;
					queues[currQ + 1].push(p);
					queues[currQ].pop();
					//queues[currQ + 1].push_back(queues[currQ].front());
					//queues[currQ].erase(queues[currQ].begin());
				}
				if(currQ == numQ - 1) {
				//aging
					p.age++;
					//if current process age reaches age limit, promote to queue above
					if(p.age == ageLimit) {
						#ifdef DEBUG
						cout << "AGE LIMIT PROMOTING " << p;
						#endif
						toNextProc = true;
						queues[currQ - 1].push(p);
						queues[currQ].pop();

						//queues[currQ - 1].push_back(queues[currQ].front());
						//queues[currQ].erase(queues[currQ].begin());
						//if new things have been added to top queue while fcfs was running, set currQ to top queue
						if(!queues[0].empty()) {
							currQ = 0;
						} else {
							currQ--;
						}
					}
				} else {
					p.age = 0;
					p.ticks++;
				}
			} else {
				currQ++;
			}
	} while(!queuesEmpty());*/
	//cout << "Average Waiting Time (AWT): " << currArrTime / numProcesses << endl;
	//cout << "Average Turnaround Time (ATT): " << totalTurnaroundTime / numProcesses << endl;
	//cout << "Total processes scheduled (NP): " << numProcesses << endl;
}

void MFQS::createQueues(int numQ){
    for(int i = 0; i < numQ; i++) {
		queues.push_back(procQueue);
	}
}

void MFQS::do_the_needful(){
    uint32_t currTick = 0;
    process p;
    p.pid = 0;
    do{   
        if(!pMap.empty()){
             #ifdef DEBUG
                 cout << "CURRTICK IS " << currTick << endl;
             #endif
             addProcessesToQueueZero(currTick);
        }
        if(!queuesEmpty()){
            p = runQueuesProcesses(p, currTick);
            if(!queues[queues.size()-1].empty()){
                ageProcesses();
            }
            currTick++;
        }
    }while(!pMap.empty() || !queuesEmpty());
    print_stats();
}

void MFQS::do_the_fcfs(){
    uint32_t currTick = 0;
    process p; 
    p.pid = 0;
    do{
        #ifdef DEBUG
            cout << "CURRTICK IS " << currTick << endl;
        #endif
        if(!pMap.empty()) 
            addProcessesToFCFSQueue(currTick);
        if(!fcfsQueue.empty()){
            p = fcfsOnlyRunProcess(p, currTick);
            currTick++;
        }
    }while(!pMap.empty() || !fcfsQueue.empty());
}

void MFQS::setCurrQueue(){
    if(queues[currQueue].size() != 0 && !queuesEmpty())
        return;
    currQueue++;
    setCurrQueue();
}

process MFQS::fcfsOnlyRunProcess(process p, uint32_t currTick){

    if(p.pid == 0){
        p = fcfsQueue.front();
        fcfsQueue.pop();
    }
    p.pTime += 1;
    if(p.pTime == p.bst){
        #ifdef DEBUG
            cout << "PROCESS " << p.pid << " FINISHED AT CURRTICK " << currTick << endl;
            cout << "THE PROCESS'S PTIME IS " << p.pTime << endl;
            cout << "THE PROCESS'S BST TIME IS " << p.bst << endl;
        #endif
            p.pid = 0;
    }
    return p;
    
}

process MFQS::runQueuesProcesses(process p, uint32_t currTick){
    if(p.pid == 0){
        p = queues[currQueue].front();
        queues[currQueue].pop();   
    }
    p.pTime +=1;
    if(p.queue != currQueue){
         cout << "PROCESS " << p.pid << " BREAKS FORMATION  AT " << currTick << " WITH QUEUE " << p.queue << "." << endl;
         cout << "SIZE OF PMAP IS " << pMap.size() << endl;
         cout << "CURRQUEE IS " << currQueue << endl;
         exit(1);
    }
    if(p.pTime == p.bst){
        #ifdef DEBUG
            cout << "PROCESS " << p.pid << " FINISHED AT CURRTICK " << currTick << endl;
            cout << "THE PROCESS'S PTIME IS " << p.pTime << endl;
            cout << "THE PROCESS's BST TIME IS " << p.bst << endl;
        #endif
        turnAround += currTick - p.arr;
        totalProcesses += 1; 
        p.pid = 0;
        currQueue = 0;
        setCurrQueue();

    }else if(quantumCheck(p) && p.queue != queues.size() - 1){
        p.queue += 1;
        queues[p.queue].push(p);
        p.pid = 0;
        currQueue = 0;
        setCurrQueue();
    }
    return p;
}

bool MFQS::quantumCheck(process p){
    int quantum = tQuantum * (2 * p.queue);
    if(quantum < tQuantum)
        quantum = tQuantum;
    if(p.pTime == quantum)
        return true;
    else
        return false;
}

void MFQS::ageProcesses(){
    int lastQueue = queues.size()-1;
    int sizeOfFCFS = queues[queues.size()-1].size();
    process p;
    for(int i = 0; i < sizeOfFCFS; i++){
        p = queues[lastQueue].front();
        p.age +=1;
        queues[lastQueue].pop();
        if(p.age == ageLimit){
            p.age = 0;
            p.queue -= 1;
            queues[p.queue].push(p);
            #ifdef DEBUG
                cout << "MOVED PROCESS " << p.pid << " TO QUEUE " << p.queue << endl;
            #endif
        }else{
            queues[lastQueue].push(p);
        }
    }
    #ifdef DEBUG
        cout << "SIZE OF FCFS QUEUE IS " << sizeOfFCFS << endl;
    #endif
}

void MFQS::addProcessesToQueueZero(uint32_t currTick){
    for(int i = 0; i < pMap[currTick].size(); i++){
        queues[0].push(pMap[currTick].at(i));
    }
    pMap.erase(currTick);
    currQueue = 0;
}

void MFQS::addProcessesToFCFSQueue(uint32_t currTick){
    for(int i = 0; i < pMap[currTick].size(); i++){
        fcfsQueue.push(pMap[currTick].at(i));
    }
    pMap.erase(currTick);
}

bool MFQS::queuesEmpty() {
	int j = 0;
	for(int i = 0; i < queues.size(); i++) {
		if(queues[i].empty()) {
			j++;
		}
	}
	return j == queues.size();
}


void MFQS::run() {
	cout <<"MFQS runnin" << endl;
}

void MFQS::print_stats(){
    cout << "Turnaround time is " << (float)(turnAround / totalProcesses) 
         << "(clocks/second)." << endl;
    cout << "Total processes is " << totalProcesses << endl;   
}
