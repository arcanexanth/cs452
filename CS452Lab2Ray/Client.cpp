#include "Client.h"

//#define DBUG

Client::Client(int port){
    this->port = port;
    this->server = "localhost";
    //Call the beginning function to start connections and sieving//
    doTheNeedful();
}

void Client::findPrime(){
    for(int i = (this->currPrime+1); i<= this->max; i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8)){
                this->currPrime = i;
                return; 
        }
    }
}

void Client::sieveComposite(){
    for(int j = (2*this->currPrime); j<= this->max; j+= this->currPrime){
        if((int) CHECK_BIT(this->bits[j/8], j%8))
                this->bits[j/8] ^= 1UL << j%8;
    }
}

void Client::doTheNeedful(){
    int sockfd = callServer(this->port);
    //cout << "Called the Server" << endl;

	int timesSent = 2;
	int timesRecd = 1;

    recvSize(sockfd);
    do{
                int totalBytes = 0;
                bool recvDone = 0;
                do{
                    int currBytes;
                    if((currBytes = recv(sockfd, bits+totalBytes, bytes, 0)) < 0){
                        error("RECV ERROR IN LOOP ON CLIENT");
                    }
                    if(currBytes == 0){
                        error("CLIENT: SERVER CLOSED CONNECTION");
                    }else{
                        totalBytes += currBytes;
                        if(totalBytes == this->bytes){
                            recvDone = 1;
                        }
                    }
                }while(!recvDone);

        readCurrPrime(sockfd);
	#ifdef DBUG
	   cout << "Recd: ";
	   setPrimes();
	   for(int i = 0; i < timesRecd; i++) {
		primes.erase(primes.begin());
	   }
		timesRecd+= 2;	

	   printPrimes();
	   primes.clear();
	#endif
	
		findPrime();
	
	#ifdef DBUG	
		cout << "Prime: " << this->currPrime << endl;
    #endif
	//cout << "FOUND PRIME IS " << this->currPrime << endl;
        sieveComposite();
   
	#ifdef DBUG    
		cout << "Sent: ";
		setPrimes();
		for(int i = 0; i < timesSent; i++) {
			primes.erase(primes.begin());
		}

		timesSent += 2;

		printPrimes();
		primes.clear();
		cout << endl << endl;
	#endif
        //send current prime back//
        
        //send the unsigned char* array back//
        send(sockfd, bits, bytes, 0);
        sendCurrPrime(sockfd);
    }while(this->currPrime <= sqrt(max));
    free(bits);

}

int Client::callServer(int port){
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
        error("ERROR CALLING SERVER");
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    struct hostent *server;
    if((server = gethostbyname(this->server))==NULL){
        error("HOST SERVER NOT FOUND!");
    }
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR CONNECTING TO SERVER");
    }
    return sockfd;
}

void Client::recvSize(int socket){
    int max;
    if(read(socket, &max, sizeof(int)) < 0){
        error("ERROR RECEIVING SIZE TO SOCKET");
    }
    this->bytes = (max/8)+1;
    this->max = max;
    this->bits = (unsigned char*)calloc(this->bytes,1);
    //cout << "RECEIVED SIZE" << endl;
}
void Client::sendCurrPrime(int socket){
    int prime = this->currPrime;
    if(write(socket, &prime, sizeof(int))<0){
        error("ERROR SENDING CURRENT PRIME TO SOCKET");
    }
}

void Client::readCurrPrime(int socket){
    //cout << "READING PRIME FROM SERVER" << endl;
    int prime = 0; 
    if(read(socket, &prime, sizeof(int))<0)
        error("ERROR RECEIVING CURRENT PRIME FROM CLIENT");
    this->currPrime = prime;
}

void Client::error(const char* msg){
    perror(msg);
    exit(0);
}

void Client::setPrimes() {
	for(int i = 0; i <= (this->max); i++) {
		if((int) CHECK_BIT(this->bits[i/8], i%8))
			this->primes.push_back(i);
	}
}

void Client::printPrimes() {
	vector<int> const &primesVec = this->primes;
	if(this->primes.size() < 6) {
		copy(primesVec.begin(), primesVec.end(), ostream_iterator<int>(cout, ", "));
		cout << endl;
	} else {
		cout << "{" << this->primes[0] << " , " << this->primes[1] << " , "
			<< this->primes[2] << " , " << " ... "  << " , "
			<< this->primes[this->primes.size()-3] << " , "
			<< this->primes[this->primes.size()-2] << " , "
			<< this->primes[this->primes.size()-1] << "}" << endl;
	}
}
