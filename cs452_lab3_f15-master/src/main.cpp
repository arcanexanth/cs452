#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <algorithm>
#include "Process.hpp"
#include "Scheduler.hpp"
#include "MFQS.hpp"
#include "RTS.hpp"
#include "WHS.hpp"

//#define DEBUG

using namespace std;
using namespace boost;

int main(int argc, char** argv) {
    
    string fileName = "";
    // makes sure the user entered just one argument
   if (argc == 2) {
       fileName = string(argv[1]);
   } else {
       cout << "Enter the file name of processes to use as an argument. (e.g. './main 100k_processes')" << endl;
       exit(1);
   }
    
    string algorithm = "";
    cout << "Which algorithm would you like to use? (mfqs/rts/whs): ";
    cin >> algorithm;
    while (algorithm != "mfqs" && algorithm != "rts" && algorithm != "whs") {
        cout << "Which algorithm would you like to use? (mfqs/rts/whs): ";
        cin >> algorithm;
    }

    
    int numberOfQueues = 0;
    int ageLimit = 0;
    int quantum = 0;
    
    if (algorithm == "mfqs") {
        // get the number of queues from the user 
        cout << "Enter how many queues: ";
        cin >> numberOfQueues;

        // get the aging wait time
        cout << "Enter wait time for aging: ";
        cin >> ageLimit;

        // get time quantum from the user
        quantum = 0;
        cout << "Enter the time quantum for the ready-to-run queue: ";
        cin >> quantum;
    }

    if (algorithm == "whs") {
        // get the aging wait time
        cout << "Enter wait time for aging: ";
        cin >> ageLimit;

        // get time quantum from the user
        quantum = 0;
        cout << "Enter the time quantum: ";
        cin >> quantum;
        if (quantum < 2) {
            cout << "Time quantum must be at least 2, or processes would not be able to do I/O." << endl;
            exit(1);
        }
    }


    // Read in the test file
    ifstream file;

    //file.open("100k_processes");
    //file.open("test2.txt");

    //file.open("test.txt");
    //file.open("1k_proc.txt");
    //file.open("10k_proc.txt");
    //file.open("10proc.txt");
    file.open(fileName);
    string line;
    vector<string> fields;
    vector<Process*> processes;
    getline(file, line); // first line is column headers (ignore)
    getline(file, line);
    // The test file MUST have a newline at the end in order for this to work.
    int pid, burst, arrival, priority, deadline, io;
    bool ignore = false;
    while (file.good()) {
        // Boost's split function, delimiter is tab character
        split(fields, line, is_any_of("\t"));

        pid = atoi(fields.at(0).c_str());
        burst = atoi(fields.at(1).c_str());
        arrival = atoi(fields.at(2).c_str());
        priority = atoi(fields.at(3).c_str());
        deadline = atoi(fields.at(4).c_str());
        io = atoi(fields.at(5).c_str());

        // ** Sanitization**
        ignore = false;

        // Ignore burst <= 0 for all schedulers
        if (burst <= 0) {
            ignore = true;
#ifdef DEBUG
            cout << "Ignoring PID " << pid << ": burst <= 0." << endl;
#endif
        }

        // Ignore arrival < 0 for all schedulers
        if (arrival < 0) {
            ignore = true;
#ifdef DEBUG
            cout << "Ignoring PID " << pid << ": arrival < 0." << endl;
#endif
        }

        // Ignore negative or early deadline only for RTS
        if ((algorithm == "rts") && ((deadline < 0) || (deadline < arrival) || (burst > deadline))) {
            ignore = true;
#ifdef DEBUG
            cout << "Ignoring PID " << pid << ": using RTS and either (deadline < 0 or deadline < arrival or burst > deadline)." << endl;
#endif
        }

        // Ignore negative I/O for WHS only
        if ((algorithm == "whs") && (io < 0)) {
            ignore = true;
#ifdef DEBUG
            cout << "Ignoring PID " << pid << ": using WHS and I/O < 0." << endl;
#endif
        }

        // Ignore duplicate PIDs
        if (!ignore) {
//             BOOST_FOREACH(Process *p, processes) {
//                 if (p->getPID() == pid) {
//                     ignore = true;
// #ifdef DEBUG
//                 cout << "Ignoring PID " << pid << ": duplicate PID." << endl;
// #endif
//                 }
//             }
        }

        if (!ignore) {
            Process *p = new Process(pid,
                                     burst, 
                                     arrival, 
                                     priority, 
                                     deadline, 
                                     io, 
                                     Process::NEW);
            processes.push_back(p);
        }
        getline(file, line);
    }
    file.close();

    // Make a vector before sorting to hold processes in order of their PID
    vector<Process*> processesByPID(processes);

    // sorts the processes by arrival time
    sort(processes.begin(), processes.end(), Process::compare);
    
    // Create the queues
    Scheduler* sched;
    if (algorithm == "mfqs") {
        sched = new MFQS(processes, quantum, numberOfQueues, ageLimit);
    } else if (algorithm == "rts") {
        sched = new RTS(processes);
    } else if (algorithm == "whs") {
        sched = new WHS(processes, processesByPID, quantum, ageLimit);
    }
    
    // Using polymorphism to call functions after "new"ing schedulers.
    sched->run();

    double avgWaitTime = sched->calcAvgWaitTime();
    cout << endl << "Average wait time: " << avgWaitTime << endl << endl;

    double avgTurnaroundTime = sched->calcAvgTurnaroundTime();

    cout << "Average turnaround time: " << avgTurnaroundTime << endl << endl;

    cout << "Total processes scheduled: " << sched->getTotalProcessesRan() << endl;

    delete sched;
    
    BOOST_FOREACH(Process *p, processes) {
        delete p;
    }

    return 0;
}

