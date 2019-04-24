//Include all the things!//
#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdint>
#include <vector>
#include <cmath>
#include <iterator>
#include <ctime>
#include <cstring>
#include "Server.h"
#include "Client.h"

using namespace std;
#define CHECK_BIT(var, pos) !!((var) &(1<<(pos)))
unsigned char* setAllToOne(unsigned char* bits, int max);
unsigned char* flipBit(unsigned char* bits, int byte, int pos);
int findPrime(unsigned char* bits, int max, int bytes, int currPrime);
unsigned char* sieveComposite(unsigned char* bits, int bytes, int max, int currPrime);
unsigned char* setupBits(unsigned char* bits, int max);
vector<int> getPrimes(unsigned char* bits, vector<int> primes, int max);
void printPrimes(vector<int> const &primes);
void local(const char **argv);
void server(const char **argv, const char* port);
void client(const char* port);

int main(int argc, const char** argv){
    const char* port = "9789";
    if(strcmp(argv[1], "Local")==0){
        if((argc == 3)){     
            local(argv);
        }else{
            cerr << "NEEDS MAX SIEVE NUMBER ARGUMENT!" << endl;
            exit(-1);
        } 
    }else if(strcmp(argv[1], "Server")==0){
        if((argc==3)){
            server(argv, port);
           
        }else{
            cerr << "NEEDS MAX SIEVE NUMBER ARGUMENT!" << endl;
            exit(-1);
        }
    }else if(strcmp(argv[1], "Client")==0){
        if(argc==2){
            client(port);
        }
    }
    cerr << "If this is 'Local', or 'Server', enter the two previous commands and a max number"<< endl;
    cerr << "If this is a 'Client' then only put in 'Client'" <<endl;
    exit(-1);
}

void server(const char **argv, const char* port){
    Server *srv = new Server(port, argv);
    delete srv;
    exit(1);
}

void client(const char* port){
    Client *cli = new Client(port);
    delete cli;
    exit(1);
}

void local(const char **argv){
    int max, bytes = 0;
    vector<int> primes;
    int currPrime = 0;
    max = atoi(argv[2]);
    bytes = atoi(argv[2])/8+1;

    //calloc the array of unsigned chars//
    unsigned char* bits = (unsigned char*)calloc(bytes, 1);
    bits = setupBits(bits, max);
    
    //start timing
    clock_t start;
    long double duration;

    do{
        currPrime = findPrime(bits, max, bytes, currPrime);
        bits = sieveComposite(bits, bytes, max, currPrime);
        currPrime++;
    }while(currPrime < sqrt(max));

    //end timing
    duration = (clock() - start)/ CLOCKS_PER_SEC;
    cout << "Time taken was: " <<duration <<endl;
    primes = getPrimes(bits, primes, max);
    free(bits);
    printPrimes(primes);
    exit(0);
}

unsigned char* setAllToOne(unsigned char* bits, int max){
    for(int i = 0; i <= max; i++){
        bits[i/8] ^= 1UL << i%8;
    }
    return bits;
}

unsigned char* flipBit(unsigned char* bits, int byte, int pos){
    bits[byte] ^= (1 << pos);
    return bits;
}

int findPrime(unsigned char* bits, int max, int bytes, int currPrime){
        for(int j = currPrime; j <= max; j++){
            if((int) CHECK_BIT(bits[j/8], j%8)){
                return (int) j;
            }
        }
}

unsigned char* sieveComposite(unsigned char* bits, int bytes, int max, int currPrime){
        for(int j = (2*currPrime); j <= max; j+=currPrime){
            if((int) CHECK_BIT(bits[j/8], j%8))
                bits[j/8] ^= 1UL << j%8;
        }   
    return bits;
}

unsigned char* setupBits(unsigned char* bits, int max){
    bits = setAllToOne(bits, max);
    bits = flipBit(bits, 0, 0);
    bits = flipBit(bits, 0, 1);
    return bits;
}

vector<int> getPrimes(unsigned char* bits, vector<int> primes, int max){
    for(int i = 0; i <= max; i++){
        if((int) CHECK_BIT(bits[i/8], i%8))
                primes.push_back(i);
    }
    return primes;
}

void printPrimes(vector<int> const &primes){
    if(primes.size() < 100){
        copy(primes.begin(), primes.end(), ostream_iterator<int>(cout, " \n"));
    }
}
