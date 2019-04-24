#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <sys/wait.h>
#include <signal.h>
#include <cmath>
#include <cerrno>
#include <iterator>
#include <cstdint>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>

//Macros//
#define CHECK_BIT(var, pos) !!((var) &(1<<(pos)))

using namespace std;
class Client{
    private:
        //private vars//
        const char* port;
        int max;
        int bytes;
        int currPrime;
        unsigned char* bits;
        char* server = "localhost";

        //private functs//
        void findPrime();
        void sieveComposite();
        void doTheNeedful();
        int callServer(int port);
        void recvSize(int socket);
        void sendCurrPrime(int socket);
        void readCurrPrime(int socket);
        void error(const char* msg);
        void printFoundPrime();

    public:
        Client(const char* port);
};
#endif

