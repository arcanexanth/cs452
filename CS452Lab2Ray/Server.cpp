#include "Server.h"

//#define DBUG

Server::Server(int port, const char **argv){
    this->port = port;
    this->max = atoi(argv[2]);
    this->bytes = (atoi(argv[2])/8)+1;
    this->bits = (unsigned char*)calloc(bytes, 1);
    
    //Call the beginning function to start connections and sieving//
    doTheNeedful();
}

void Server::setNeededToOne(){
    for(int i = 2; i <=this->max; i++){
        this->bits[i/8] ^= 1UL << i%8;
    }
}

void Server::findPrime(){
    for(int i = (this->currPrime+1); i<= this->max; i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8)){
                this->currPrime = i;
                return; 
        }
    }
}

void Server::sieveComposite(){
    for(int j = (2*this->currPrime); j<= this->max; j+= this->currPrime){
        if((int) CHECK_BIT(this->bits[j/8], j%8))
                this->bits[j/8] ^= 1UL << j%8;
    }
}

void Server::doTheNeedful(){
    setNeededToOne();
    int sockfd = beginListener(this->port);
    int socket = acceptClient(sockfd);
    //cout << "ACCEPTED CLIENT!" << endl;
    sendSize(socket);
    clock_t start;
    long double duration;
  	
	vector<int> readPrimes;

	int timesSent = 1;
	int timesRecd = 2;	
	do{
        findPrime();
        //cout << "FOUND PRIME IS " << this->currPrime << endl;
        sieveComposite();
//		cout << "Prime: " << this->currPrime << endl;
  
	#ifdef DBUG	
		cout << "Prime: ";
		  		//send size of needed unsigned char* array//
		setPrimes();
		for(int i = 0; i < timesSent; i++) {
				readPrimes.push_back(primes[i]);
		}
		if(readPrimes.size() < 6) {
			for(int i = 0; i < timesSent; i++) {
				cout << readPrimes[i] << ", ";
			}
		} else {
			for(int i = 0; i < 3; i++) {
				cout << readPrimes[i] << ", ";
			}
			cout << "... , ";
			for(int i = readPrimes.size()-3 ; i < readPrimes.size();  i++) {
				cout << readPrimes[i] << ", ";
			}

		}
		readPrimes.clear();
		cout << endl;
	#endif

	
	#ifdef DBUG
		cout << "Sent: ";
		
		for(int i = 0; i < timesSent; i++) {
			primes.erase(primes.begin());
		}
		
		timesSent += 2;

		printPrimes();
		primes.clear();	

		cout << endl << endl;
	#endif
        
        send(socket, bits, bytes, 0);
		sendCurrPrime(socket);
		
            int totalBytes = 0;
            bool recvDone = 0;
            do{
                int currBytes;
                if((currBytes = recv(socket, bits+totalBytes, bytes, 0)) < 0){
                    error("RECV ERROR IN LOOP ON SERVER");
                }
                if(currBytes == 0){
                    error("SERVER: CLIENT CLOSED CONNECTION");
                }else{
                    totalBytes += currBytes;
                    if(totalBytes == this->bytes){
                        recvDone = 1;
                    }
                }
            }while(!recvDone);
            
        readCurrPrime(socket);
	
    #ifdef DBUG
		cout << "Recd: ";
		setPrimes();

		for(int i = 0; i < timesRecd; i++) {
			primes.erase(primes.begin());
		}

		timesRecd += 2;

		printPrimes();
		primes.clear();

	#endif
    }while(this->currPrime <= sqrt(max));

    duration = (clock() - start)/CLOCKS_PER_SEC;
    cout << "Time take was: " << duration << endl;
    
	cout << "All primes: ";
	setPrimes();
    free(bits);
    printPrimes();

}

int Server::beginListener(int port){
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd< 0){
            error("ERROR OPENING SOCKET");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR ON BINDING!");
    }

    listen(sockfd,5);
    return sockfd;
}

int Server::acceptClient(int serverSocket){
    int newsockfd;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    cout << "Waiting for a call..." << endl;
    newsockfd = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0){
        error("ERROR ON ACCEPTING CONNECTION!");
    }
    return newsockfd;
}

void Server::sendSize(int socket){
    int max = this->max;
    //cout << "SENDING SIZE" << endl;
    if(write(socket, &max, sizeof(int)) < 0){
        error("ERROR SENDING SIZE TO SOCKET");
    }
}
void Server::sendCurrPrime(int socket){
    int currPrime = this->currPrime;
    if(write(socket, &currPrime, sizeof(int))<0){
        error("ERROR SENDING CURRENT PRIME TO CLIENT");
    }
}

void Server::readCurrPrime(int socket){
    //cout << "READING PRIME FROM CLIENT" << endl;
    int prime = 0;
    if(read(socket, &prime, sizeof(int))<0)
        error("ERROR GETTING CURRENT PRIME FROM CLIENT");
    this->currPrime = prime;
}

void Server::setPrimes(){
    for(int i= 0; i <=(this->max); i++){
        if((int) CHECK_BIT(this->bits[i/8], i%8))
            this->primes.push_back(i);
    }
}

void Server::error(const char* msg){
    perror(msg);
    exit(1);
}

void Server::printPrimes(){
    vector<int> const &primesVec = this->primes;
    if(this->primes.size() < 6){
        copy(primesVec.begin(), primesVec.end(), ostream_iterator<int>(cout, ", "));
    	cout << endl;
	}else{
        cout << "{" << this->primes[0] << " , " << this->primes[1] << " , " 
                << this->primes[2] << " , " << " ... "  << " , " 
                << this->primes[this->primes.size()-3] << " , " 
                << this->primes[this->primes.size()-2] << " , "
                << this->primes[this->primes.size()-1] << "}" << endl;
    }

}



