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
#include <stdint.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>

//Macros//
#define CHECK_BIT(var, pos) !!((var) &(1<<(pos)))

using namespace std;
class Client{
    private:
        //private vars//
        int port;
        int max;
        int bytes;
        int currPrime;
        unsigned char* bits;
        const char* server;
	vector<int> primes;


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

	void setPrimes();
	void printPrimes();

    public:
        Client(int port);
};
#endif

