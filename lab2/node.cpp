#include <iostream>
#include <cmath>
#include "uwecSocket.h"


#define CHECK_BIT(var, pos) !!((var) &(1<<(pos)))
int findPrime(unsigned char* bits, int max, int bytes, int currPrime);
unsigned char* sieveComposite(unsigned char* bits, int bytes, int max, int currPrime);

int main() {
	int portno = 9419;

	int sockfd = callServer("localhost", portno);
	printf("Node connected\n");

	/***SETTING UP INITIAL EMPTY ARRAY***/
	cout << "reading max from machine A" << endl;
	int max = readInt(sockfd);
	cout << "max: " << max << endl;

	cout << "reading bytes from machine A" << endl;
	int bytes = readInt(sockfd);
	cout << "bytes: " << bytes << endl;

	unsigned char* bits = (unsigned char*) calloc(bytes, 1);

	/***EMPTY ARRAY DONE***/

	int currPrime = 0;
	while(currPrime < sqrt(max)) {
		cout << "reading currPrime from machine B" << endl;
		currPrime = readInt(sockfd);

		cout << "reading array from machine A" << endl;
		recv(sockfd, bits, bytes, 0);
	
		currPrime = findPrime(bits, max, bytes, currPrime);
		bits = sieveComposite(bits, bytes, max, currPrime);
		currPrime++;
	
		cout << "sending array to machine A" << endl;
		send(sockfd, bits, bytes, 0);

		cout << "sending currPrime to machine A" << endl;
		writeInt(currPrime, sockfd);
	}
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
