#ifndef PROCESS_HPP
#define	PROCESS_HPP
#include <string>

using namespace std;

class Process {
    
public:
    enum State {
    	NEW,
    	READY_TO_RUN,
    	WAITING_FOR_IO,
    	RUNNING,
    	TERMINATED
    };
    Process();
    Process(int pid, int burst, int arrival_time, int priority, int deadline, int io, State state);
    Process(const Process &p);
    virtual ~Process();
    string toString() const;
    State getState() const;
    void setState(State s);
    int getArrivalTime() const;
    int getBurst() const;
    int getTimeRemaining() const;
    void setTimeRemaining(int t);
    int getPriority() const;
    int getPID() const;
    int getDeadline() const;
    int getIO() const;
    int getTimeWaiting() const;
    void addTimeWaiting(int time);
    int getExitCPUTick() const;
    void setExitCPUTick(int clock);
    int getFinishTime() const;
    void setFinishTime(int clock);
    int getAge() const;
    void addAge(int age);
    void setAge(int age);
    static bool compare(Process *p1, Process *p2);
    static bool compareDeadline(Process *p1, Process *p2);
    static bool comparePID(Process* p1, Process* p2);
    static int compare_priority_tree(void* p1, void* p2);
    int getOriginalPriority() const;
    bool incrementPriority(int amt);
    bool decrementPriority(int amt);
    bool isHighBand() const;
    int getIOTimer() const;
    void resetIOTimer();
    void decrementIOTimer();
    void decrementTimeRemaining();

private:
    int pid;
    int burst;
    int arrival_time;
    int priority;
    int deadline;
    int io;
    State state;
    int time_remaining;
    int time_waiting;
    int age;
    int exitCPUTick;
    int finishTime;
    int originalPriority;
    bool highBand;
    int ioTimer;
};

#endif

