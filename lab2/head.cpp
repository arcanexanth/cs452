#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdint>
#include <cmath>
#include <iterator>
#include <cstring>
#include <vector>
#include "uwecSocket.h"

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
void client(const char **argv, const char* port);


int main(int argc, char *argv[]) {
	int portno = 9419;
	int sockfd = setupServerSocket(portno);
	int start = 10;
	int end = 100;
	int socket;
	
	if(argv[1] == NULL) {
		cout << "null" << endl;
	}
	cout << argv[1] << endl;

	//wait for all nodes to connect then send work

	socket = serverSocketAccept(sockfd);
	printf("All nodes connected\n");

	int max, bytes = 0;
	vector<int> primes;
	int currPrime = 0;
	max = atoi(argv[1]);
	bytes = atoi(argv[1])/8 + 1;

	cout << "sending max to machine B" << endl;
	writeInt(max, socket);

	cout << "sending bytes to machine B" << endl;
	writeInt(bytes, socket);



	//calloc the array of unsigned chars
	unsigned char* bits = (unsigned char*) calloc(bytes, 1);
	bits = setupBits(bits, max);

	while(currPrime < sqrt(max)) {
		currPrime = findPrime(bits, max, bytes, currPrime);
		bits = sieveComposite(bits, bytes, max, currPrime);
		currPrime++;
		
		cout << "sending currPrime to machine B" << endl;
		writeInt(currPrime, socket);

		cout << "sending array to machine B" << endl;

		send(socket, bits, bytes, 0);

		cout << "reading array from machine B" << endl;
		recv(socket, bits, bytes, 0);

		cout << "reading currPrime from machine B" << endl;
	
	}


	primes = getPrimes(bits, primes, max);
	free(bits);

	printPrimes(primes);
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

