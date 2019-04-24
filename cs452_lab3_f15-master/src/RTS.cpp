#include <iostream>
#include <algorithm>
#include "Process.hpp"
#include "RTS.hpp"
#include "Time_Queue.hpp"
#include "Scheduler.hpp"

//#define DEBUG

using namespace std;

RTS::RTS(vector<Process *> &processes)
        : Scheduler(processes){
}

RTS::~RTS() {
}

//Note: This was first edited in VIM, then edited in Notepad++ later

void RTS::run() {
  int clock = 0;
  int jobsLoaded = 0; //keeps track of how many processes have entered the process ecosystem. points at which process should be loaded next
  int jobsRan = 0; //counts how many processes have had a chance to run
  bool hardRealTimeStop = false; //true if process didn't finish using hard real time mode
  char softOrHard; //records user decision to use soft or hard real time mode (stop when a process can't be scheduled, or keep going if that happens)
  
  //need to create some data structure to hold processes loaded in (making priority queue compatible proved too complex)
  //a C++ deque should do.
  deque<Process*> rtsQueue; //contains processes that have "arrived" sorted by deadline
  Process *p; //currently active process

  //if there's nothing, don't bother trying to populate the deque
  if(this->hasUnfinishedJobs()){
  
    //prompt user: soft or hard real time and verbose or not
    cout << "Chose soft or hard real time mode (s/h): ";
    cin >> softOrHard;
	cout << endl;
	
	//in case the arrival time of the earliest arriving process isn't 0, advance the clock to when it does arrive
	if(this->processes.at(jobsLoaded)->getArrivalTime() != clock){
	  clock = this->processes.at(jobsLoaded)->getArrivalTime();
	}
    
    //"while there is a process to load and arrival time is equal to clock,
    //"load them into the rtsQueue and set their state to READY"
    while(jobsLoaded < (int)this->processes.size() && this->processes.at(jobsLoaded)->getArrivalTime() == clock){
#ifdef DEBUG
      cout << "loaded " << this->processes.at(jobsLoaded)->getPID() << endl; //TESTING: display PIDs of first process(es) loaded in
#endif
      //we should see if processes may be able to complete within the deadline
      //I'm wondering if we should try to predict if a process won't finish due to so many other processes being loaded...
      if(this->processes.at(jobsLoaded)->getDeadline() >= this->processes.at(jobsLoaded)->getBurst()){ //burst < deadline
        rtsQueue.push_back(this->processes.at(jobsLoaded)); //process is added to rtsQueue, making it READY
        this->processes.at(jobsLoaded)->setState(Process::READY_TO_RUN);
        jobsLoaded++;
		
      /* This is redundant since the main function handles processes with negative deadlines, very low deadlines, or busrt > deadline
	    } else { //process won't be able to finish, so terminate it
	    this->processes.at(jobsLoaded)->setState(Process::TERMINATED);
	    jobsLoaded++;*/
      }
    }

    //sort rtsQueue by deadline (possibly slow, but 100k processes finishes within 3 minutes without output, so it's not so bad)
    sort(rtsQueue.begin(), rtsQueue.end(), Process::compareDeadline);

  //in preparation to run RTS, pop the first process off of rtsQueue (earliest PID with shortest deadline)
  p = rtsQueue.front();
  rtsQueue.pop_front();
  
  }
  
  //We're done when all processes are terminated either from having too short a deadline or finishing its burst
  while(this->hasUnfinishedJobs() && !hardRealTimeStop){
  
#ifdef DEBUG
    //output clock and currently active process info (occasionally?)
	//if(clock % 10 == 0){
      cout << "Clock: " << clock << ". Process: PID: " << p->getPID() << ", Time Remaining: " << p->getTimeRemaining() << ", Deadline: " << p->getDeadline() << endl;
	//}
#endif
    if(softOrHard == 'h' && p->getTimeRemaining() + clock > p->getDeadline()){ //hard real time mode is active. stop scheduler loop now if a process can't finish
	  hardRealTimeStop = true;
	  cout << "Process " << p->getPID() << " cancelled. Shutting down." << endl;
	  
	} else {

		while(p != NULL && p->getTimeRemaining() + clock > p->getDeadline()){ //remove any and all processes that can't finish by deadline
		  p->setState(Process::TERMINATED); //terminates current active process. since p is a pointer, it is set as terminated in the scheduler's list as well
#ifdef DEBUG		  
		  cout << "Process " << p->getPID() << " cancelled. Clock: " << clock << " Time Remaining: " << p->getTimeRemaining() 
				<< " Deadline: " << p->getDeadline() << endl; //output info of cancelled process
#endif		  
		  //Count a process as having ran for calculating average wait/turnaround times
		  if(p->getTimeRemaining() != p->getBurst()){
		    jobsRan++;
		  }
		  
		  //load the next process, if there is one in rtsQueue
		  if(rtsQueue.size() > 0){
			p = rtsQueue.front();
			rtsQueue.pop_front();
		  } else {
			p = NULL;
		  }
		  
#ifdef DEBUG
		  cout << "rtsQueue: ";
		  for (int i = 0; i < (int)rtsQueue.size(); i++) { //TESTING: display PIDs of processes loaded into rtsQueue
			cout << rtsQueue.at(i)->getPID() << " ";
		  }
		  cout << endl;
#endif
		  
		}
	

		//CLOCK TICK, reduce time remaining on active process
		if(p != NULL){
		  p->setTimeRemaining(p->getTimeRemaining() - 1);
		}
	
		//increment wait times of all processes loaded in thus far
		for(int i = 0; i < (int)rtsQueue.size(); i++){
		  Process* p2 = rtsQueue.at(i);
		  p2->addTimeWaiting(1);
		}

		//advance the clock, last part of CLICK TICK (occurs with or without an active process)
		clock++;
#ifdef DEBUG
		if(clock == 100000){
		  cout << "Possibly stuck. Current process PID: " << p->getPID() << endl; //TESTING: shows if we may be in an infinite loop
		}
#endif
		//check if the current process is done (if one is active)
		if(p != NULL && p->getTimeRemaining() == 0){

		  p->setFinishTime(clock);
		  p->setState(Process::TERMINATED);
		  jobsRan++;
#ifdef DEBUG		  
		  cout << p->getPID() << " ended." << endl; //TESTING: output PID of process that just ended
#endif
		  //pop next process from rtsQueue, if there is one
		  if(rtsQueue.size() > 0){
			p = rtsQueue.front();
			rtsQueue.pop_front();
			
		  } else if(jobsLoaded < (int)this->processes.size()){  //otherwise, advance the clock to when the next process arrives and empty p so it doesn't go back into rtsQueue
            clock = this->processes.at(jobsLoaded)->getArrivalTime();
			p = NULL;
		  }
		  
		  
#ifdef DEBUG
		  cout << "rtsQueue: ";
		  for (int i = 0; i < (int)rtsQueue.size(); i++) { //TESTING: display PIDs of processes loaded into rtsQueue
			cout << rtsQueue.at(i)->getPID() << " ";
		  }
		  cout << endl;
#endif
		  
		//if there is no active process, we can advance the clock immediately to the arrival time of the next process to be loaded in (if there's one to load)
		} else if(p == NULL && jobsLoaded < (int)this->processes.size()){ 
		  clock = this->processes.at(jobsLoaded)->getArrivalTime();
		}



	  //check if a process has "arrived" (unless we loaded them all)
	  if(jobsLoaded < (int)this->processes.size() && this->processes.at(jobsLoaded)->getArrivalTime() == clock){
	  //This was omitted: causes errors when p is null
	  //cout << "Process " << this->processes.at(jobsLoaded)->getPID() << " has arrived. Deadlines: " << p->getDeadline() << " " << this->processes.at(jobsLoaded)->getDeadline() << endl;

	  //put currently active process (if we have one) back in the queue in case it needs to be replaced with a process with a shorter deadline. We'll end up pulling it back out if it doesn't
	  if(p != NULL){
	    rtsQueue.push_back(p);
	  }

#ifdef DEBUG
		  cout << "rtsQueue: ";
		  for (int i = 0; i < (int)rtsQueue.size(); i++) { //TESTING: display PIDs of processes loaded into rtsQueue
			cout << rtsQueue.at(i)->getPID() << " ";
		  }
		  cout << endl;
#endif

		while(jobsLoaded < (int)this->processes.size() && this->processes.at(jobsLoaded)->getArrivalTime() == clock){

			if(this->processes.at(jobsLoaded)->getDeadline() >= this->processes.at(jobsLoaded)->getBurst() + clock){ //process can finish by deadline

			  rtsQueue.push_back(this->processes.at(jobsLoaded));
			  this->processes.at(jobsLoaded)->setState(Process::READY_TO_RUN);
			  jobsLoaded++;

			} else { //process can't finish by deadline

			  if(softOrHard == 'h'){ //hard real time mode: stop everything
			    hardRealTimeStop = true;
	            cout << "Process " << this->processes.at(jobsLoaded)->getPID() << " cancelled. Shutting down." << endl;
				clock = -1; //forces while loop to end
#ifdef DEBUG	  
	            //output more info of cancelled process
	            cout << "Process details are, Clock: " << clock << " Time Remaining: " << p->getTimeRemaining() << " Deadline: " << p->getDeadline() << endl;
#endif
			  } else { //soft real time mode: report this process and load the next one
#ifdef DEBUG
		        cout << "Process " << this->processes.at(jobsLoaded)->getPID() << " not scheduled. Clock: " << clock << " Time Remaining: " << this->processes.at(jobsLoaded)->getTimeRemaining() 
				  << " Deadline: " << this->processes.at(jobsLoaded)->getDeadline() << endl; //output info of cancelled process
#endif
			    this->processes.at(jobsLoaded)->setState(Process::TERMINATED);
			    jobsLoaded++;
			  }
			}

		  
	    }
		
		//Order processes by deadline again and make the lowest deadline process active (unless hard real time is stopping RTS)
		if(!hardRealTimeStop){
		  sort(rtsQueue.begin(), rtsQueue.end(), Process::compareDeadline);
		  p = rtsQueue.front();
		  rtsQueue.pop_front();
		}
      }
    }
  }
  //sending info about the number of jobs that got loaded into rtsQueue goes here
  this->totalProcessesRan = jobsRan;
}
