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
#include <chrono>
#include <thread>
#include "RTS.h"

//Defines//
#ifndef DBUG
//#define DBUG

void error(const char* msg);
size_t getFilesize(const char* file);

using namespace std;

int main (int argc, char** argv){
    //Variables for processes//
    const char* file = "./500k";
    char *buf;
    process_map pMap;
    #ifdef DBUG
        cout << "pMap max size is: " << pMap.max_size() << endl;
    #endif
    chrono::system_clock::time_point now = chrono::system_clock::now();

    //Variables for mmap()//
    size_t size = getFilesize(file);
    int fd = open(file, O_RDONLY, 1);

    if(fd == -1)
        error("ERROR OPENING FILE");

    char* data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_POPULATE , fd, 0);
    char* dataend;
    char* line = strtok_r(data, "\n", &dataend);
    vector<char*> fields;
    int maxPsize = 0;

    while(line != NULL){

        int i = 0;
        char* blockend;
        char* block= strtok_r(line, "\t", &blockend);
        while(block != NULL){
            fields.push_back(block);
            i++;
            block = strtok_r(NULL, "\t", &blockend);
        }
       if((strcmp(fields[0],"Pid")!=0) && (atoi(fields[1])>0) && (atoi(fields[2])>=0) 
            && (atoi(fields[3])>=0) && (atoi(fields[4])>0) && (atoi(fields[5])>=0) && ((atoi(fields[4]) - (atoi(fields[2]) + atoi(fields[1])))>0)){
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

            #ifdef DBUG
               cout << "pMap current size is: " << pMap.size() << endl;
               cout << p.pid << " ";
               cout << p.bst << " ";
               cout << p.arr << " ";
               cout << p.ddLine << " ";
               cout << p.io << endl;
               cout << "SIZE OF PROCESS IS " << sizeof(p) << endl;
            #endif

            if(sizeof(p) > maxPsize)
                maxPsize = sizeof(p);

            //Add process to map//
            pMap[p.arr].push_back(p);
        }
        //Empty fields for new data//
        fields.clear();

        //Get new line//
        line = strtok_r(NULL, "\n", &dataend);
    }



    if(munmap(data, size) == -1)
        error("ERROR UNMAPPING DATA");
    close(fd);

    //call the different schedulers after this//

    RTS *rts = new RTS(pMap);
    //#ifdef DBUG
        cout << "duration is " << chrono::system_clock::to_time_t(chrono::system_clock::now())
                - chrono::system_clock::to_time_t(now) << endl;
        //cout << "Size of pMap is " << pMap.size() << endl;
        cout << "Largest process is a size of " << maxPsize << " bytes." << endl;
    //#endif   
    delete(rts);
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
