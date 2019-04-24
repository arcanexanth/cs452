#include "RTS.h"
#ifndef DBUG
//#define DBUG

RTS::RTS(process_map pMap){
    this->pMap = pMap;
    cout << "Is this a hard RTS or soft RTS?" << endl;
    char* hard = (char*)calloc(1, sizeof(char*));
    cin >> hard;
    //strcpy(hard, "soft");
    if(strcmp(hard, "hard")==0)
            this->hardRTS = true;

    free(hard);
    doTheNeedful();
}

void RTS::queueProcesses(){
    for(int i = 0; i < pMap[currTick].size(); i++){
        #ifdef DBUG
        //    cout << "PROCESS ADDED TO QUEUE IS " << pMap[currTick].at(i).pid << endl;
        #endif
        //processes set = pMap[currTick];
        pQueue.push(pMap[currTick].at(i));
        #ifdef DBUG
            pCopy.push(pMap[currTick].at(i));
        #endif
    }

    pMap.erase(currTick);
    #ifdef DBUG
//    if(!pMap.empty())
//        cout << "pMap size is now " << pMap.size() << endl;
    #endif

}

void RTS::doTheNeedful(){
    do{
        queueProcesses();
        #ifdef DBUG
            if(pCopy.size() > 0)
                print_queue(pCopy);
        #endif

        if(pQueue.size() > 0){
            runProcess();
        }
        if(increaseCurrTick){
            currTick++;
            increaseCurrTick = false;
        }

        #ifdef DBUG
           cerr << "CURRTICK IS " << currTick << endl;
        #endif
    }while(!pQueue.empty() || !pMap.empty());
    
    print_stats();
}

void RTS::runProcess(){
       
    if(!pQueue.empty()&& !((int)(pQueue.top().ddLine - (currTick + pQueue.top().bst - pQueue.top().pTime))<0)){
        increaseCurrTick = true;
        #ifdef DBUG 
            cout << endl;
            cout << "CURRENT TICKS IS AT " << currTick << endl;
            cout << "QUEUE SIZE IS " << pQueue.size() << endl;
            cout << "PID IS " << pQueue.top().pid << endl;
            cout << "DDLINE IS " << pQueue.top().ddLine << endl;
            cout << "BST IS " << pQueue.top().bst << endl;
            cout << "PTIME IS " << pQueue.top().pTime << endl;
            cout << "DDLINE RESULT IS " << (int)(pQueue.top().ddLine - 
                      (currTick + pQueue.top().bst - pQueue.top().pTime)) << endl; 
        #endif
        process p = pQueue.top();
        pQueue.pop();
        p.pTime += 1;
        pQueue.push(p);
        #ifdef DBUG
            cout << "PROCESS " << pQueue.top().pid << " HAD IT'S PTIME INCREASED AND IS NOW " << pQueue.top().pTime << endl;
            cout << endl;
        #endif
       if(pQueue.top().pTime == pQueue.top().bst){
            this->processed += 1;
            this->turnAround += currTick - p.arr;
            #ifdef DBUG
                cout << "PROCESS DELETED WAS " << pQueue.top().pid << endl;
            #endif
            pQueue.pop();
        }
    }else{
            failed += 1;
            error(" THE PROCESS WILL NOT COMPLETE BY DEADLINE", pQueue.top());
            pQueue.pop();
            increaseCurrTick = false;
    }  
}

bool RTS::compareByDeadline(const process &a, const process &b){
    if(a.ddLine == b.ddLine)
            return a.pid < b.pid;
    return a.ddLine < b.ddLine; 
}

void RTS::error(const char* msg, process p){
    #ifdef DBUG
        cout << msg << p.pid << endl;
        cout << "DDLINE IS " << p.ddLine << endl;
        cout << "BST IS " << p.bst << endl;
        cout << "PTIME IS " << p.pTime << endl;
        cout << "CURRENT TICK IS " << currTick << endl;
        cout << endl;
    #endif
    if(hardRTS)
        exit(1);
}

void RTS::print_stats(){
    cout << "TOTAL PROCESSED IS " << processed << endl;
    cout << "TOTAL FAILED IS " << failed << endl;
    cout << "TOTAL PROCESSES " << processed + failed << endl;
    cout << "AVG TURNAROUND IS " << (float) (turnAround/processed) << "clocks/second." << endl;
}

template<typename A> void RTS::print_queue(A& q){
    
    cout << "QUEUE IS " << endl;
    while(!pCopy.empty()){
        cout << "PID IS: " << pCopy.top().pid <<endl;
        cout << "DDLINE IS: " << pCopy.top().ddLine << endl;
        pCopy.pop();
    }
    cout << " " << endl;
}
#endif
