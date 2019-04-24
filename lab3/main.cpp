// constructing unordered_multimaps
#include <iostream>
#include <string>
#include <unordered_map>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include "MFQS.h"
#include "RTS.h"
//Defines//
#ifndef DBUG
//#define DBUG


void error(const char* msg);
size_t getFilesize(const char* file);

using namespace std;

int main (int argc, char** argv){
    const char* file = "./500k";
//    const char* file = "./10proc.txt";
    char *buf;
    process_map pMap;
    #ifdef DBUG
        cout << "pMap max size is: " << pMap.max_size() << endl;
    #endif
    size_t size = getFilesize(file);
    int fd = open(file, O_RDONLY, 1);

    if(fd == -1)
        error("ERROR OPENING FILE");

    char* data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE, fd, 0);
    char* dataend;
    char* line = strtok_r(data, "\n", &dataend);
    vector<char*> fields;
    while(line != NULL){
        int i = 0;
        char* blockend;
        char* block= strtok_r(line, "\t", &blockend);
        while(block != NULL){
	    fields.push_back(block);
            i++;
            block = strtok_r(NULL, "\t", &blockend);
        }
        #ifdef DBUG
            cout << "EXITED BLOCK LOOP" << endl;
        #endif
        if((strcmp(fields[0],"Pid")!=0) && (atoi(fields[1])>0) && (atoi(fields[2])>=0)){
            #ifdef DBUG
                cout << "Inside da fields if check" << endl;
            #endif
            process p;
                p.pid = atoi(fields[0]);
                p.bst = atoi(fields[1]);
                p.arr = atoi(fields[2]);
                p.pri = atoi(fields[3]);
                p.ddLine = atoi(fields[4]);
                p.io = atoi(fields[5]);
                p.ticks = 0;
                p.pTime = 0;
                p.age = 0;
                p.queue = 0;

            #ifdef DBUG
               cout << "pMap current size is: " << pMap.size() << endl;
               cout << p.pid << " ";
               cout << p.bst << " ";
               cout << p.arr << " ";
               cout << p.pri << " ";
	       cout << p.ddLine << " ";
               cout << p.io << endl;
            #endif
               pMap[p.arr].push_back(p);
               
        }
	fields.clear();
        line = strtok_r(NULL, "\n", &dataend);
    }
if(munmap(data, size) == -1)
        error("ERROR UNMAPPING DATA");
    close(fd);

	cout << "Which scheduler? (1 MFQS, 2 RTS, 3 WHS)" << endl;
	int sched;
	cin >> sched;


	switch(sched) {

	{case 1:
		int numQ = 0;
		bool validQ = false;
		while(!validQ) {
			cout << "How many queues? (1-5)" << endl;
			cin >> numQ;

			if(numQ > 0 && numQ < 6) {
				validQ = true;
			}
		}
	
		int quantum;
		cout << "Time quantum?" << endl;
		cin >> quantum;
	
		int ageLimit;
		cout << "Aging time?" << endl;
		cin >> ageLimit;
	
		//quantum, numQueue, ageLimit
		MFQS* foo = new MFQS(pMap, quantum, numQ, ageLimit);
		delete(foo);
		break;
	}
	{case 2:
         RTS* rts = new RTS(pMap);
         delete(rts);
         break;
	}
	{case 3:
		//WHS
		/*int quantum;
		cout << "Time quantum?" << endl;
		cin >> quantum;*/
        cerr << "WHS WAS NOT IMPLEMENTED!" << endl;
	    break;
	}
	{default:
         cerr << "INVALID NUMBER!" << endl;
		
	}
	}
  return 0;
}

void error(const char* msg){
    perror(msg);
    exit(1);
}

size_t getFilesize(const char* filename){
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

#endif
