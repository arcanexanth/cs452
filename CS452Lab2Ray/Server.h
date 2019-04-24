#ifndef SERVER_H
#define SERVER_H

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
#include <ctime>

//Macros//
#define CHECK_BIT(var, pos) !!((var) &(1<<(pos)))

using namespace std;
class Server{
    private:
        //private vars//
        int port;
        int max;
        int bytes;
        int currPrime;
        unsigned char* bits;
        vector<int> primes;

        //private functs//
        void setNeededToOne();
        //unsigned char* flipBit(unsigned char* bits, int byte, int pos);
        void findPrime();
        void sieveComposite();
        //unsigned char* setupBits(unsigned char* bits, int max);
        void setPrimes();
        void doTheNeedful();
        int beginListener(int port);
        int acceptClient(int serverSocket);
        void sendSize(int socket);
        void sendCurrPrime(int socket);
        void readCurrPrime(int socket);
        void error(const char* msg);
        void printPrimes();

    public:
        Server(int port, const char **argv);

};
#endif

